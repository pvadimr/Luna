#ifndef VEHICLE_H
#define VEHICLE_H
#include "../config.h"
#ifdef PHYSICS
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Physics.h"
#include "../Scene/Scene.h"
#include "../Events/InGameEventReceiver.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class	btCollisionShape;
class	btDynamicsWorld;
class	btRigidBody;
class	btTypedConstraint;

//! Vehicle states.
enum EVEHICLE_STATE
{
  EVEHICLE_ACCELERATING = 726956,
  EVEHICLE_REVERSING,
  EVEHICLE_STALLED
};

class Vehicle{

	public:

    Vehicle();
    virtual ~Vehicle();

   // IrrlichtDevice *intro;

    //! Updates wheel transformations, calls updateVehicleMovement.
    void renderme();

    void updateVehicleMovement(void);
    void initPhysics(stringc,stringc);
    void registerCamera(ICameraSceneNode *camera2);

    scene::IMetaTriangleSelector* metaSelector;
    ICameraSceneNode* camera;
    scene::ITriangleSelector* selector;

    IEventReceiver* m_cInGameEvents;
    void setEventRec(IEventReceiver* m_event);

    void recursiveFillMetaSelector(scene::ISceneNode* node, scene::IMetaTriangleSelector* meta );

    scene::ISceneNodeAnimatorCollisionResponse* anim;

    void createTerrain(vector3df pos,char *heightmap);

    void loadLevel(const stringc file);

    void accelerate(f32 speed);

    void brake(void);

    void reverse(f32 speed);

    void steer_left(void);

    void steer_right(void);

    void steer_reset(void);

    void resetVehicle(void);

        void setPosition( core::vector3df fun);


    void registerScene(Scene &scene);

    vector3df getPosition(void);


    void setState(EVEHICLE_STATE _state);

    EVEHICLE_STATE getState(void);

    // Irrlicht variables.
    IrrlichtDevice *m_irrDevice;
    void registerIrrDevice(IrrlichtDevice &device);
    #ifdef PHYSICS
    void registerPhysics(Physics &physics);
        btQuaternion getVehicleOrientation(void);


    //
    Physics *m_cPhysics;
    Scene *m_cScene;
        btVector3   btCarScale;


            void SetParams(
            float scaleT2,
            float scaleE2,
            btVector3 btCarScale2,
            btVector3 btModelscale2,
            btVector3 m_vehiclePosition2,
            int vehicleWeight2,
            float maxBreakingForce2,
            float maxEngineForce2,
            float SpeedINC2,
            int driveType2,
            float connectionHeight2,
            float suspensionStiffness2,
            float suspensionDamping2,
            float suspensionCompression2,
        //    float suspensionRestLength2,
            float rollInfluence2,
            float wheelFriction2,
            float wheelRadius2,
            float wheelWidth2,
            float steeringIncrement2,
            float steeringClamp2,
            float CUBE_HALF_EXTENTS2 ,
            float wheelSpacingX2,
            float wheelScaleFactor2
    );
btRigidBody*        m_carChassis;
  private:

    btBroadphaseInterface*	    m_overlappingPairCache;
    btTriangleIndexVertexArray*	m_indexVertexArrays;

    btRaycastVehicle::btVehicleTuning	m_tuning;

    btTypedConstraint*	m_pickConstraint;	//constraint for mouse picking
    btCollisionShape*	  m_shootBoxShape;
    btVehicleRaycaster*	m_vehicleRayCaster;
    btRaycastVehicle*	  m_vehicle;

    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

    // These variables are retrieved from Physics class, and should not be created in Vehicle.
		//btDiscreteDynamicsWorld *m_dynamicsWorld;
		//btDefaultCollisionConfiguration *m_collisionConfiguration;
		//btCollisionDispatcher *m_dispatcher;
		//btSequentialImpulseConstraintSolver *m_constraintSolver;


    btVector3*	m_vertices;
    btVector3   m_cameraPosition;
    btVector3   m_cameraTargetPosition;//look at
    btVector3   m_cameraUp;
    btVector3   m_vehiclePosition;
    btVector3   btModelscale;
    btVector3   btModelrotation;
    btVector3 wheelDirectionCS0;
    btVector3 wheelAxleCS;
    btScalar suspensionRestLength;

    btTransform tr;

    bool	m_stepping;
    bool  m_singleStep;
    bool  m_idle;

    int   maxProxies;
    int   maxOverlap;

    int   m_lastKey;
    int	  m_forwardAxis;
    int	  m_debugMode;
    int   m_numWheels;

    int vehicleWeight;

    float	m_cameraDistance;
    float m_ele;
    float m_azi;
    float	m_ShootBoxInitialSpeed;
    float m_scaleBottom;
    float m_scaleFactor;
    float	m_cameraHeight;
    float	m_minCameraDistance;
    float	m_maxCameraDistance;

    int driveType;
    f32 SpeedINC;
    f32	gEngineForce;
    f32	gBreakingForce;
    f32	maxEngineForce;
    f32	maxBreakingForce;
    float	gVehicleSteering;
    float	steeringIncrement;
    float	steeringClamp;
    float   wheelScaleFactor;
    float	wheelRadius;
    float	wheelWidth;
    float	wheelFriction;
    float	suspensionStiffness;
    float	suspensionDamping;
    float	suspensionCompression;
    float	rollInfluence;
    float   connectionHeight;
    float   CUBE_HALF_EXTENTS;
    f32 factor; // for wheels
    f32 wheelSpacingX;
    f32 wheelSpacingZ;
    f32 wheelSpacingZ2; // Front wheels z-shift




    //! Current state of the vehicle.
    EVEHICLE_STATE state;

    array<IAnimatedMeshSceneNode *> wheelNode;

    // Coordiante variables.
    int upIndex;
    int rightIndex;
    int forwardIndex;
};
    #endif
#endif

#endif
