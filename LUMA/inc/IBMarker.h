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
#ifndef IBMARKER_H
#define IBMARKER_H

#include "stdafx.h"
#include "Marker.h"

/// \brief	Immersed boundary marker.
///
///			This class declaration is for an immersed boundary Lagrange point.
///			A collection of these points form an immersed boundary body.
class IBMarker : public Marker {

	// Make ObjectManager a friend class so it can access the protected data of IBMarker objects
	friend class ObjectManager;
	// Same for IBBody
	friend class IBBody;
	friend class IBInfo;

public:

	/// Default constructor
	IBMarker(void)
	{
		position.push_back(0.0);
		position.push_back(0.0);
		position.push_back(0.0);

		supp_i.push_back(0);
		supp_j.push_back(0);
		supp_k.push_back(0);

#ifdef L_BUILD_FOR_MPI
		support_rank.push_back(MpiManager::getInstance()->my_rank);
#else
		support_rank.push_back(0);
#endif

		// TODO: Initialise the rest of the IB Marker properties here. 

	};
	/// Default destructor
	~IBMarker(void)
	{
	};

	// Custom constructor to add support etc.
	IBMarker(double xPos, double yPos, double zPos, GridObj const * const body_owner, bool isFlexible = false);

protected:

	/************** Member Data **************/
	
	// General vectors
	std::vector<double> fluid_vel;		///< Fluid velocity interpolated from lattice nodes
	std::vector<double> desired_vel;	///< Desired velocity at marker
	std::vector<double> force_xyz;		///< Restorative force vector on marker
	std::vector<double> position_old;	///< Vector containing the physical coordinates (x,y,z) of the marker at t-1. Used for moving bodies.

	// Support quantities
	std::vector<double> deltaval;		///< Value of delta function for a given support node

	// Scalars
	bool isFlexible;		///< Indication as to whether marker is part of a structural or moving body calculation
	double epsilon;			///< Scaling parameter
	double local_area;		///< Area associated with support node in lattice units (same for all points if from same grid and regularly spaced like LBM)
	double dilation;		///< Dilation parameter in lattice units (same in all directions for uniform Eulerian grid)

};

#endif