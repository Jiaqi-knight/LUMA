/*
 * --------------------------------------------------------------
 *
 * ------ Lattice Boltzmann @ The University of Manchester ------
 *
 * -------------------------- L-U-M-A ---------------------------
 *
 *  Copyright (C) 2015, 2016
 *  E-mail contact: info@luma.manchester.ac.uk
 *
 * This software is for academic use only and not available for
 * distribution without written consent.
 *
 */
#ifndef OBJMAN_H
#define OBJMAN_H

#include "stdafx.h"
#include "IVector.h"
#include "IBInfo.h"
#include "IBMarker.h"
#include "IBBody.h"
#include "BFLBody.h"

class PCpts;
class GridObj;


/// \enum  eObjectType
/// \brief Specifies the type of body being processed.
enum eObjectType {
	eBBBCloud,	///< Bounce-back body
	eBFLCloud,	///< BFL body
	eIBBCloud	///< Immersed boundary body
};


/// \brief	Object Manager class.
///
///			Class to manage all objects in the domain from creation through 
///			manipulation to destruction.
class ObjectManager
{

	// Make Grid a friend so boundary conditions can access the body data
	friend class GridObj;

	/* Members */

private:

	// Bounce-back object fields
	double forceOnObjectX = 0.0;			///< Instantaneous X-direction force on BB bodies in domain
	double forceOnObjectY = 0.0;			///< Instantaneous Y-direction force on BB bodies in domain
	double forceOnObjectZ = 0.0;			///< Instantaneous Z-direction force on BB bodies in domain

	// Objects
	std::vector<Body<Marker>> bBody;		///< Array of default bodies
	std::vector<IBBody> iBody;				///< Array of immersed boundary bodies
	std::vector<BFLBody> pBody;				///< Array of BFL bodies

	/// Pointer to grid hierarchy
	GridObj* _Grids;

	/// Pre-stream distribution functions for applying BFL BCs
	IVector<double> f_prestream;

	/// Pointer to self
	static ObjectManager* me;


	/* Methods */

private:
	ObjectManager(void);		///< Private constructor
	~ObjectManager(void);		///< Private destructor
	/// Overloaded constructor to set pointer to grid hierarchy
	ObjectManager(GridObj* _Grids);		

public:
	// Singleton design
	static ObjectManager *getInstance();			///< Get instance method
	static void destroyInstance();					///< Destroy instance method
	static ObjectManager *getInstance(GridObj* g);	///< Overloaded get instance passing in pointer to grid hierarchy

	// IBM methods //
	void ibm_apply();						// Apply interpolate, compute and spread operations for all bodies.
	void ibm_buildBody(int body_type);		// Build a new pre-fab IBM body
	void ibm_buildBody(PCpts* _PCpts, GridObj *owner);		// Overloaded build-body to build from point cloud
	void ibm_initialise();					// Initialise a built immersed body with support.
	double ibm_deltaKernel(double rad, double dilation);	// Evaluate kernel (delta function approximation).
	void ibm_interpol(int ib);				// Interpolation of velocity field onto markers of ib-th body.
	void ibm_spread(int ib);				// Spreading of restoring force from ib-th body.
	void ibm_findSupport(int ib, int m);	// Populates support information for the m-th marker of ib-th body.
	void ibm_initialiseSupport(int ib, int m, 
		int s, double estimated_position[]);		// Initialises data associated with the support points.
	void ibm_computeForce(int ib);			// Compute restorative force at each marker in ib-th body.
	double ibm_findEpsilon(int ib);			// Method to find epsilon weighting parameter for ib-th body.
	void ibm_moveBodies();					// Update all IBBody positions and support.
	double ibm_bicgstab(std::vector< std::vector<double> >& Amatrix,
		std::vector<double>& bVector, std::vector<double>& epsilon,
						   double tolerance, int maxiterations);	// Biconjugate gradient stablised method for solving asymmetric 
																	// linear system required by finding epsilon

	// IBM-MPI methods //
	void ibm_mpi_pack(eIBInfoType type, IBBody *iBody, std::vector<int> &buffer);
	void ibm_mpi_pack(eIBInfoType type, IBBody *iBody, std::vector<double> &buffer);

	void ibm_mpi_unpack(eIBInfoType type, std::vector<int> &bufferRecv, int bufferSendSize, std::vector<int> &rankID, std::vector<int> &numMarkers);
	void ibm_mpi_unpack(eIBInfoType type, std::vector<std::vector<double>> &bufferRecv, std::vector<int> &rankID, std::vector<int> &numMarkers, std::vector<std::vector<double>> &markerPos);

	void ibm_getNumMarkers(eIBInfoType type, IBBody *iBody, std::vector<int> &rankID, std::vector<int> &numMarkers);
	void ibm_getMarkerPositions(eIBInfoType type, IBBody *iBody, std::vector<std::vector<double>> &markerPos, std::vector<int> &rankID, std::vector<int> &numMarkers);


	// Flexible body methods
	void ibm_jacowire(int ib);					// Computes the tension and position of a 2D inextensible, flexible filament.
	void ibm_positionUpdate(int ib);			// Updates the position of deformable body markers.
	void ibm_positionUpdateGroup(int group);	// Updates the positions of deformable bodies in a group.
	// Methods to solve the Jacobian system associated with Jacowire
	void ibm_banbks(double **a, long n, int m1, int m2, double **al,
		unsigned long indx[], double b[]);
	void ibm_bandec(double **a, long n, int m1, int m2, double **al,
		unsigned long indx[], double *d);


	// BFL methods //
	void bfl_buildBody(int body_type);		// Build a new pre-fab bounce-back body
	void bfl_buildBody(PCpts* _PCpts);		// Overload to build from point cloud data

	// Force calculation
	void computeLiftDrag(int i, int j, int k, GridObj *g);		// Compute force for BBB or BFLB residing on supplied grid.

	// IO methods //
	void io_vtkIBBWriter(double tval);				// VTK body writer
	void io_writeBodyPosition(int timestep);		// Write out IBBody positions at specified timestep to text files
	void io_writeLiftDrag(int timestep);			// Write out IBBody lift and drag at specified timestep
	void io_restart(eIOFlag IO_flag, int level);	// Restart read and write for IBBodies given grid level
	void io_readInCloud(PCpts* _PCpts, eObjectType objtype);	// Method to read in Point Cloud data
	void io_writeForceOnObject(double tval);		// Method to write object forces to a csv file
};

#endif
