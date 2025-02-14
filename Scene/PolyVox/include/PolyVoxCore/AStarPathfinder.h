/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*******************************************************************************/
#include "../../../../config.h"
#ifdef POLYVOX
#ifndef __PolyVox_AStarPathfinder_H__
#define __PolyVox_AStarPathfinder_H__

#include "Impl/AStarPathfinderImpl.h"
#include "Impl/TypeDef.h"

#include "PolyVoxCore/Array.h"

#include <list>
#include <stdexcept> //For runtime_error

namespace PolyVox
{
	const float sqrt_1 = 1.0f;
	const float sqrt_2 = 1.4143f;
	const float sqrt_3 = 1.7321f;

	extern const POLYVOX_API Vector3DInt32 arrayPathfinderFaces[6];
	extern const POLYVOX_API Vector3DInt32 arrayPathfinderEdges[12];
	extern const POLYVOX_API Vector3DInt32 arrayPathfinderCorners[8];

	/// This function provides the default method for checking whether a given voxel
	/// is valid for the path computed by the AStarPathfinder.
	template<typename VolumeType>
	bool aStarDefaultVoxelValidator(const VolumeType* volData, const Vector3DInt32& v3dPos);

	/// Provides a configuration for the AStarPathfinder.
	////////////////////////////////////////////////////////////////////////////////
	/// This structure stores the AStarPathfinder%s configuration options, because this
	/// is simpler than providing a large number of get/set properties within the
	/// AStarPathfinder itself. In order to create an instance of this structure you
	/// must provide at least a volume, a start and end point, and a list to store
	/// the result. All the other option have sensible default values which can
	/// optionally be changed for more precise control over the pathfinder's behaviour.
	///
	/// \sa AStarPathfinder
	////////////////////////////////////////////////////////////////////////////////
	template<typename VolumeType>
	struct AStarPathfinderParams
	{
	public:
		AStarPathfinderParams
		(
			VolumeType* volData,
			const Vector3DInt32& v3dStart,
			const Vector3DInt32& v3dEnd,
			std::list<Vector3DInt32>* listResult,
			float fHBias = 1.0,
			uint32_t uMaxNoOfNodes = 10000,
			Connectivity requiredConnectivity = TwentySixConnected,
			polyvox_function<bool (const VolumeType*, const Vector3DInt32&)> funcIsVoxelValidForPath = &aStarDefaultVoxelValidator,
			polyvox_function<void (float)> funcProgressCallback = 0
		)
			:volume(volData)
			,start(v3dStart)
			,end(v3dEnd)
			,result(listResult)
			,connectivity(requiredConnectivity)
			,hBias(fHBias)
			,maxNumberOfNodes(uMaxNoOfNodes)
			,isVoxelValidForPath(funcIsVoxelValidForPath)
			,progressCallback(funcProgressCallback)
		{
		}

		/// This is the volume through which the AStarPathfinder must find a path.
		VolumeType* volume;

		/// The start point for the pathfinding algorithm.
		Vector3DInt32 start;

		/// The end point for the pathfinding algorithm.
		Vector3DInt32 end;

		/// The resulting path will be stored as a series of points in
		/// this list. Any existing contents will be cleared.
		std::list<Vector3DInt32>* result;

		/// The AStarPathfinder performs its search by examining the neighbours
		/// of each voxel it encounters. This property controls the meaning of
		/// neighbour - e.g. whether two voxels must share a face, edge, or corner.
		Connectivity connectivity;

		/// For each voxel the pathfinder tracks its distance to the start (known as g())
		/// and estimates its distance to the end (known as h()). Increasing or decreasing
		/// h() has an effect on the way the pathfinder behaves. If h() is an underestimate
		/// of the true distance then the pathfinder will act more like a greedy search -
		/// always finding the shortest path but taking longer to do so. If h() is an over
		/// estimate then the pathfinder will behave more like a best-first search - returning
		/// a potentially suboptimal path but finding it more quickly. The hBias is multiplied
		/// by the estimated h() value to control this behaviour.
		float hBias;

		/// Volumes can be pretty huge (millions of voxels) and processing each one of these
		/// can take a long time. In A* terminology each voxel is a node, and this property
		/// controls the maximum number of nodes that will be considered when finding the path,
		/// before giving up and throwing an exception because a path can't be found.
		uint32_t maxNumberOfNodes;

		/// This function is called to determine whether the path can pass though a given voxel. The
		/// default behaviour is specified by aStarDefaultVoxelValidator(), but users can specify thier
		/// own criteria if desired. For example, if you always want a path to follow a surface then
		/// you could check to ensure that the voxel above is empty and the voxel below is solid.
		///
		/// \sa aStarDefaultVoxelValidator
		polyvox_function<bool (const VolumeType*, const Vector3DInt32&)> isVoxelValidForPath;

		/// This function is called by the AStarPathfinder to report on its progress in getting to
		/// the goal. The progress is reported by computing the distance from the closest node found
		/// so far to the end node, and comparing this with the distance from the start node to the
		/// end node. This progress value is guarenteed to never decrease, but it may stop increasing
		///for short periods of time. It may even stop increasing altogether if a path cannot be found.
		polyvox_function<void (float)> progressCallback;
	};

	/// The AStarPathfinder compute a path from one point in the volume to another.
	////////////////////////////////////////////////////////////////////////////////
	/// A* is a well known pathfinding algorithm commonly used in computer games. It
	/// takes as input a pair of points in the world, and works out a path between
	/// them which avoids obstacles and adheres to other user defined criteria. The
	/// resulting path is usually the shortest possible, but a less optimal path can
	/// be exchanged for reduced computation time.
	///
	/// For an excellent overview of the A* algorithm please see Amit Patel's Game
	/// Programming page here: http://theory.stanford.edu/~amitp/GameProgramming/
	/// Much of this class is based on the principles described in those pages.
	///
	/// Usage of this class if very strightforward. You create an instance of it
	/// by passing an instance of the AStarPathfinderParams structure to the constructor.
	/// The details of the AStarPathfinderParams and the options it provides are described
	/// in the documentation for that class.
	///
	/// Next you call the execute() function and wait for it to return. If a path is
	/// found then this is stored in the list which was set as the 'result' field of
	/// the AStarPathfinderParams. If a path cannot be found then an exception of type
	/// std::runtime_error is thrown.
	///
	/// \sa AStarPathfinderParams
	////////////////////////////////////////////////////////////////////////////////
	template<typename VolumeType>
	class AStarPathfinder
	{
	public:
		AStarPathfinder(const AStarPathfinderParams<VolumeType>& params);

		void execute();

	private:
		void processNeighbour(const Vector3DInt32& neighbourPos, float neighbourGVal);

		float SixConnectedCost(const Vector3DInt32& a, const Vector3DInt32& b);
		float EighteenConnectedCost(const Vector3DInt32& a, const Vector3DInt32& b);
		float TwentySixConnectedCost(const Vector3DInt32& a, const Vector3DInt32& b);
		float computeH(const Vector3DInt32& a, const Vector3DInt32& b);
		uint32_t hash(uint32_t a);

		//Node containers
		AllNodesContainer allNodes;
		OpenNodesContainer openNodes;
		ClosedNodesContainer closedNodes;

		//The current node
		AllNodesContainer::iterator current;

		float m_fProgress;

		AStarPathfinderParams<VolumeType> m_params;
	};
}

#include "PolyVoxCore/AStarPathfinder.inl"

#endif //__PolyVox_AStarPathfinder_H__
#endif
