#pragma once

#include "Vector3.h"
#include "Grid.h"
#include "EntityBase.h"
#include "../FPSCamera.h"

class CSpatialPartition
{
protected:
    static CSpatialPartition *sp_instance;
    // Constructor
    CSpatialPartition(void);

    // Variables
    CGrid* theGrid;
    int xSize;
    int ySize;
    int zSize;
    int xGridSize;
    int yGridSize;
    int zGridSize;
    int xNumOfGrid;
    int yNumOfGrid;
    int zNumOfGrid;
    Vector3 Offset;
    std::string _meshName; // Name of the mesh
    std::string _meshName2; // Name of the mesh 2

    // We store the pointer to the Camera so we can get it's position and direction to calculate LOD and visibility
    FPSCamera* theCamera;
    // LOD distances
    float LevelOfDetails_Distances[2];

public:
    static CSpatialPartition *GetInstance()
    {
        if (!sp_instance)
            sp_instance = new CSpatialPartition;
        return sp_instance;
    }
    static bool DropInstance()
    {
        if (sp_instance)
        {
            delete sp_instance;
            sp_instance = NULL;
            return true;
        }
        return false;
    }
    ~CSpatialPartition(void);

    // Initialise the spatial partition
    bool Init(const int xGridSize, const int yGridSize, const int zGridSize,
        const int xNumOfGrid, const int yNumOfGrid, const int zNumOfGrid);

    // Set a particular grid's Mesh
    void SetMesh(const std::string& _meshName, const std::string& _meshName2);

    // ApplyMesh
    void ApplyMesh(void);

    // Update the spatial partition
    void Update(void);
    // Render the spatial partition
    void Render(Vector3* theCameraPosition = NULL);

    // Get xSize of the entire spatial partition
    int GetxSize(void) const;
    // Get ySize of the entire spatial partition
    int GetySize(void) const;
    // Get zSize of the entire spatial partition
    int GetzSize(void) const;
    // Get xSize
    int GetxGridSize(void) const;
    // Get ySize
    int GetyGridSize(void) const;
    // Get zNumOfGrid
    int GetzGridSize(void) const;
    // Get xNumOfGrid
    int GetxNumOfGrid(void) const;
    // Get yNumOfGrid
    int GetyNumOfGrid(void) const;
    // Get zNumOfGrid
    int GetzNumOfGrid(void) const;

    // Get Offset
    Vector3 GetOffset(void) const;

    // Get a particular grid
    CGrid GetGrid(const int xIndex, const int yIndex, const int zIndex) const;

    // Get vector of objects from this Spatial Partition
    vector<EntityBase*> GetObjects(Vector3 position, const float radius);

    // Add a new object
    void Add(EntityBase* theObject);
    // Remove but not delete object from this grid
    void Remove(EntityBase* theObject);

    // Calculate the squared distance from camera to a grid's centrepoint
    float CalculateDistanceSquare(Vector3* theCameraPosition, const int xIndex, const int yIndex, const int zIndex);

    //PrintSelf
    void PrintSelf() const;

    // The vector of objects due for migration to another grid
    vector<EntityBase*> MigrationList;

    // Handling Camera
    void SetCamera(FPSCamera* _cameraPtr);
    void RemoveCamera(void);

    // Set LOD camera
    void SetLevelOfDetails(const float distance_High2Mid, const float distance_Mid2Low);
    // Check if a CGrid is visible to the camera
    bool IsVisible(Vector3 theCameraPosition, Vector3 theCameraDirection, const int xIndex, const int yIndex, const int zIndex);
};
