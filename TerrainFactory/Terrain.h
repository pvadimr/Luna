#ifndef TERRAIN_H
#define TERRAIN_H
#include "../config.h"
#ifdef TERRAIN
#include <deque>

//#include "../vector.h"
#include "tilegrid.h"
#include <irrlicht.h>
using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
#ifdef PHYSICS
#include "../Physics/Physics.h"
#endif
#include "../Scene/Scene.h"

#ifdef TREES
#include "ProceduralTrees/CTreeGenerator.h"
#include "ProceduralTrees/CBillboardGroupSceneNode.h"
#include "ProceduralTrees/CTreeSceneNode.h"
#endif

#include <vector>
using namespace std;

class NODE
{
	public:
		int          bType;
        core::vector3df	     vBoundingCoordinates[4];
    	unsigned int uiBranches[4];
		unsigned int uiID;
		unsigned int uiParentID;
		/*
			unsigned int uiVertexStrip1[8]; // 4 vertex per quad
			unsigned int uiVertexStrip2[8];
			unsigned int uiVertexStrip3[8];
			unsigned int uiVertexStrip4[8];
		*/
};


class Terrain
{
	public:
		Terrain();
		~Terrain();

		char pName[20];
		int pSeed[10];
        //NODE* pNode;
        NODE* pNodeList;


        // Irrlicht variables.
            IrrlichtDevice *m_irrDevice;
            Scene *m_cScene;
            #ifdef PHYSICS
            Physics *m_cPhysics;
			btTransform tr;
			        btBvhTriangleMeshShape *trackShape;
			    void registerPhysics(Physics &physics);
			btRigidBody*	localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape, ISceneNode *node);
            		btRigidBody* mRigidBody;
            #endif
            scene::ISceneManager* smgr;
            video::IVideoDriver* driver;



    void registerIrrDevice(IrrlichtDevice &device);

    void registerScene(Scene &scene);

            //SMeshBuffer* pNode1 ;
            //SMeshBuffer* glvertex;
        float 	     *fVerts;
        unsigned int *uiList;
        SMesh * cubeMesh;
        ISceneNode * cubeSceneNode;


		unsigned int CalcNodeNum ( unsigned int max,unsigned int min );
		void CreateGrid ( unsigned int w,unsigned int h );
		void GenerateNode ( NODE* pNode, const int &TN ,const int &XT, const int &ZT, const unsigned int &LOD=1);
		void CreateNode ( unsigned int Bounding[4],unsigned int ParentID,unsigned int NodeID );
		void Init();
		void generateLod(unsigned int x, NODE* pNode, const int TN, const int XT, const int ZT);
		float getHeight ( float x,float z );
        float getHeight2 ( float x,float z );
//		void Sphere ( int dtheta, int dphi );
//		friend class Azadi;
		static int setter;
		ITerrainSceneNode* Terrain2(vector3df ,vector3df ,char *hmap,char *tex,char* dmap);

		ITerrainSceneNode* terrain;

    void Render(char*,vector3df terrainPosition,vector3df terrainRotation,vector3df terrainScale,int LOD);
    #ifdef TREES
    CTreeSceneNode* MakeTrees(vector3df aha,int treetype,char *);
#endif



	//	void tick(const float &CamX, const float &CamY, const float &CamZ);
	private:
   //     float extentX, extentY, extentZ;
  //     float nextentX, nextentY, nextentZ;
//        std::deque<std::deque<SceneGraphNodes::TerrainNode*> > tNodes;

//		Frustum *frustum;
//		std::vector<GLfloat> glvertex;
 //       std::vector<GLuint> elements;
//		GLuint *vertElements;
//		GLfloat *vertexData;

  //  vector <CTreeSceneNode*> tree2;
		unsigned int sizeVerts;
		unsigned int sizeElements;

//		TextureFactory *texture_factory;
//		std::vector<std::vector<NODE*> > pNode1;
//		std::vector<std::vector<std::vector<NODE *> > > planets;
};

#endif
#endif
