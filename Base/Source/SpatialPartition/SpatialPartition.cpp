#include "SpatialPartition.h"
#include "stdio.h"
#include "Collider\Collider.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../LevelOfDetails/LevelOfDetails.h"

template <typename T> vector<T> concat(vector<T> &a, vector<T> &b) {
    vector<T> ret = vector<T>();
    copy(a.begin(), a.end(), back_inserter(ret));
    copy(b.begin(), b.end(), back_inserter(ret));
    return ret;
}

// Allocating and initializing CPlayerInfo's static data member.  
// The pointer is allocated but not the object's constructor.
CSpatialPartition *CSpatialPartition::sp_instance = 0;

/********************************************************************************
Constructor
********************************************************************************/
CSpatialPartition::CSpatialPartition(void)
    : theGrid(NULL)
    , xSize(0)
    , ySize(0)
    , zSize(0)
    , xGridSize(0)
    , yGridSize(0)
    , zGridSize(0)
    , xNumOfGrid(0)
    , yNumOfGrid(0)
    , zNumOfGrid(0)
    , Offset(Vector3(0, 0, 0))
    , _meshName("")
    , theCamera(NULL)
{
}

/********************************************************************************
Destructor
********************************************************************************/
CSpatialPartition::~CSpatialPartition(void)
{
    theCamera = NULL;
    delete[] theGrid;
}

/********************************************************************************
Initialise the spatial partition
********************************************************************************/
bool CSpatialPartition::Init(const int xGridSize, const int yGridSize, const int zGridSize,
    const int xNumOfGrid, const int yNumOfGrid, const int zNumOfGrid)
{
    if ((xGridSize>0) && (yGridSize>0) && (zGridSize>0)
        && (xNumOfGrid>0) && (yNumOfGrid>0) && (zNumOfGrid>0))
    {
        this->xNumOfGrid = xNumOfGrid;
        this->yNumOfGrid = yNumOfGrid;
        this->zNumOfGrid = zNumOfGrid;
        this->xGridSize = xGridSize;
        this->yGridSize = yGridSize;
        this->zGridSize = zGridSize;
        this->xSize = xGridSize * xNumOfGrid;
        this->ySize = yGridSize * yNumOfGrid;
        this->zSize = zGridSize * zNumOfGrid;
        this->Offset.x = xGridSize >> 1;
        this->Offset.y = yGridSize >> 1;
        this->Offset.z = zGridSize >> 1;
        // Create an array of grids
        theGrid = new CGrid[xNumOfGrid*yNumOfGrid*zNumOfGrid];
        // Initialise the array of grids
        for (int i = 0; i<xNumOfGrid; i++)
        {
            for (int j = 0; j<zNumOfGrid; j++)
            {
                for (int k = 0; k < yNumOfGrid; k++)
                {
                    theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].Init(i, k, j, xGridSize, yGridSize, zGridSize, (xSize >> 1), (ySize >> 1), (zSize >> 1));
                }
            }
        }
        // Assign a Mesh to each Grid if available.
        ApplyMesh();
        // Create a migration list vector
        MigrationList.clear();
        return true;
    }
    return false;
}

/********************************************************************************
Set a particular grid's Mesh
********************************************************************************/
void CSpatialPartition::SetMesh(const std::string& _meshName, const std::string& _meshName2)
{
    this->_meshName = _meshName;
    this->_meshName2 = _meshName2;

    // Assign a Mesh to each Grid if available.
    ApplyMesh();
}

/********************************************************************************
ApplyMesh
********************************************************************************/
void CSpatialPartition::ApplyMesh(void)
{
    if (_meshName.size() != 0)
    {
        for (int i = 0; i<xNumOfGrid; i++)
        {
            for (int j = 0; j<zNumOfGrid; j++)
            {
                for (int k = 0; k < yNumOfGrid; ++k)
                {
                    vector<EntityBase*> test = theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].GetListOfObject();
                    if (test.size() > 0)
                        theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].SetMesh(_meshName2);
                    else 
                        theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].SetMesh(_meshName);
                }
            }
        }
    }
}

/********************************************************************************
Update the spatial partition
********************************************************************************/
void CSpatialPartition::Update(void)
{
    //ApplyMesh();

    for (int i = 0; i<xNumOfGrid; i++)
    {
        for (int j = 0; j<zNumOfGrid; j++)
        {
            for (int k = 0; k < yNumOfGrid; ++k)
            {
                theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].Update(&MigrationList);

                // Check visibility
                if (IsVisible(theCamera->GetCameraPos(),
                    theCamera->GetCameraTarget() - theCamera->GetCameraPos(),
                    i, k, j) == true)
                {
                    // Calculate LOD for this CGrid
                    float distance = CalculateDistanceSquare(&(theCamera->GetCameraPos()), i, j, k);
                    if (distance < LevelOfDetails_Distances[0])
                    {
                        theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].SetDetailLevel(CLevelOfDetails::HIGH_DETAILS);
                    }
                    else if (distance < LevelOfDetails_Distances[1])
                    {
                        theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].SetDetailLevel(CLevelOfDetails::MID_DETAILS);
                    }
                    else
                    {
                        theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].SetDetailLevel(CLevelOfDetails::LOW_DETAILS);
                    }
                }
                else
                    theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].SetDetailLevel(CLevelOfDetails::NO_DETAILS);
            }
        }
    }
    // If there are objects due for migration, then process them
    if (MigrationList.empty() == false)
    {
        // Check each object to see if they are no longer in this grid
        for (int i = 0; i < MigrationList.size(); ++i)
        {
            Add(MigrationList[i]);
        }
        MigrationList.clear();
    }
}

/********************************************************************************
Render the spatial partition
********************************************************************************/
void CSpatialPartition::Render(Vector3* theCameraPosition)
{
    // Render the Spatial Partitions
    MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
    modelStack.PushMatrix();
    //modelStack.Translate(xOffset, yOffset, zOffset);
    for (int i = 0; i<xNumOfGrid; i++)
    {
        for (int j = 0; j<zNumOfGrid; j++)
        {
            for (int k = 0; k < yNumOfGrid; k++)
            {
                modelStack.PushMatrix();
                modelStack.Translate(xGridSize*i - (xSize >> 1), yGridSize*k - (ySize >> 1), zGridSize*j - (zSize >> 1));
                modelStack.PushMatrix();
                modelStack.Scale(xGridSize, yGridSize, zGridSize);
                theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].Render();
                modelStack.PopMatrix();
                modelStack.PopMatrix();
            }
        }
    }
    modelStack.PopMatrix();
}

/********************************************************************************
Get xSize of the entire spatial partition
********************************************************************************/
int CSpatialPartition::GetxSize(void) const
{
    return xSize;
}
/********************************************************************************
Get ySize of the entire spatial partition
********************************************************************************/
int CSpatialPartition::GetySize(void) const
{
    return ySize;
}
/********************************************************************************
Get zSize of the entire spatial partition
********************************************************************************/
int CSpatialPartition::GetzSize(void) const
{
    return zSize;
}
/********************************************************************************
Get xSize
********************************************************************************/
int CSpatialPartition::GetxGridSize(void) const
{
    return xGridSize;
}
/********************************************************************************
Get ySize
********************************************************************************/
int CSpatialPartition::GetyGridSize(void) const
{
    return yGridSize;
}
/********************************************************************************
Get zSize
********************************************************************************/
int CSpatialPartition::GetzGridSize(void) const
{
    return zGridSize;
}
/********************************************************************************
Get xNumOfGrid
********************************************************************************/
int CSpatialPartition::GetxNumOfGrid(void) const
{
    return xNumOfGrid;
}
/********************************************************************************
Get yNumOfGrid
********************************************************************************/
int CSpatialPartition::GetyNumOfGrid(void) const
{
    return yNumOfGrid;
}
/********************************************************************************
Get zNumOfGrid
********************************************************************************/
int CSpatialPartition::GetzNumOfGrid(void) const
{
    return zNumOfGrid;
}
/********************************************************************************
Get Offset
********************************************************************************/
Vector3 CSpatialPartition::GetOffset(void) const
{
    return Offset;
}
/********************************************************************************
Get a particular grid
********************************************************************************/
CGrid CSpatialPartition::GetGrid(const int xIndex, const int yIndex, const int zIndex) const
{
    return theGrid[(xIndex*zNumOfGrid + zIndex) + (yIndex * xNumOfGrid * zNumOfGrid)];
}

/********************************************************************************
Get vector of objects from this Spatial Partition
********************************************************************************/
vector<EntityBase*> CSpatialPartition::GetObjects(Vector3 position, const float radius)
{
    // Get the indices of the object's position
    int xIndex = (((int)position.x - (-xSize >> 1)) / (xSize / xNumOfGrid));
    int yIndex = (((int)position.y - (-ySize >> 1)) / (ySize / yNumOfGrid));
    int zIndex = (((int)position.z - (-zSize >> 1)) / (zSize / zNumOfGrid));

    return theGrid[(xIndex*zNumOfGrid + zIndex) + (yIndex * xNumOfGrid * zNumOfGrid)].GetListOfObject();
}

/********************************************************************************
Add a new object model
********************************************************************************/
void CSpatialPartition::Add(EntityBase* theObject)
{
    // Get the indices of the object's position
    int xIndex = (((int)theObject->GetPosition().x - (-xSize >> 1)) / (xSize / xNumOfGrid));
    int yIndex = (((int)theObject->GetPosition().y - (-ySize >> 1)) / (ySize / yNumOfGrid));
    int zIndex = (((int)theObject->GetPosition().z - (-zSize >> 1)) / (zSize / zNumOfGrid));
    // Add them to each grid
    if (((xIndex >= 0) && (xIndex<xNumOfGrid)) && ((yIndex >= 0) && (yIndex<yNumOfGrid)) && ((zIndex >= 0) && (zIndex<zNumOfGrid)))
    {
        theGrid[(xIndex*zNumOfGrid + zIndex) + (yIndex * xNumOfGrid * zNumOfGrid)].Add(theObject);
    }
}

// Remove but not delete object from this grid
void CSpatialPartition::Remove(EntityBase* theObject)
{
    /*
    // Get the indices of the object's position
    int xIndex = (((int)theObject->GetPosition().x - (-xSize >> 1)) / (xSize / xNumOfGrid));
    int zIndex = (((int)theObject->GetPosition().z - (-zSize >> 1)) / (zSize / zNumOfGrid));

    // Add them to each grid
    if (((xIndex >= 0) && (xIndex<xNumOfGrid)) && ((zIndex >= 0) && (zIndex<zNumOfGrid)))
    {
    theGrid[xIndex*zNumOfGrid + zIndex].Remove(theObject);
    }
    */
}

/********************************************************************************
Calculate the squared distance from camera to a grid's centrepoint
********************************************************************************/
float CSpatialPartition::CalculateDistanceSquare(Vector3* theCameraPosition, const int xIndex, const int yIndex, const int zIndex)
{
    float xDistance = (xGridSize * xIndex + (xGridSize >> 1) - (xSize >> 1)) - theCameraPosition->x;
    float yDistance = (yGridSize * yIndex + (yGridSize >> 1) - (ySize >> 1)) - theCameraPosition->y;
    float zDistance = (zGridSize * zIndex + (zGridSize >> 1) - (zSize >> 1)) - theCameraPosition->z;

    return (float)(xDistance*xDistance + yDistance*yDistance + zDistance*zDistance);
}

/********************************************************************************
Set Camera
********************************************************************************/
void CSpatialPartition::SetCamera(FPSCamera* _cameraPtr)
{
    this->theCamera = _cameraPtr;
}

/********************************************************************************
Remove Camera
********************************************************************************/
void CSpatialPartition::RemoveCamera(void)
{
    this->theCamera = NULL;
}

/********************************************************************************
Set LOD distances
********************************************************************************/
void CSpatialPartition::SetLevelOfDetails(const float distance_High2Mid, const float distance_Mid2Low)
{
    LevelOfDetails_Distances[0] = distance_High2Mid;
    LevelOfDetails_Distances[1] = distance_Mid2Low;
}

/********************************************************************************
Check if a CGrid is visible to the camera
********************************************************************************/
bool CSpatialPartition::IsVisible(Vector3 theCameraPosition, Vector3 theCameraDirection, const int xIndex, const int yIndex, const int zIndex)
{
    float xDistance = (xGridSize * xIndex + (xGridSize >> 1) - (xSize >> 1)) - theCameraPosition.x;
    float yDistance = (yGridSize * yIndex + (yGridSize >> 1) - (ySize >> 1)) - theCameraPosition.y;
    float zDistance = (zGridSize * zIndex + (zGridSize >> 1) - (zSize >> 1)) - theCameraPosition.z;
    // If the camera is within the CGrid, then display by default
    // Otherwise, the entity may not get displayed
    if (xDistance * xDistance + yDistance * yDistance + zDistance * zDistance < (xGridSize * xGridSize + yGridSize * yGridSize + zGridSize * zGridSize))
        return true;
    Vector3 gridCentre(xDistance, yDistance, zDistance);
    if (theCameraDirection.Dot(gridCentre) < 0)
        return false;
    return true;
}

/********************************************************************************
PrintSelf
********************************************************************************/
void CSpatialPartition::PrintSelf() const
{
    cout << "******* Start of CSpatialPartition::PrintSelf() **********************************" << endl;
    cout << "xSize\t:\t" << xSize << "\ySize\t:\t" << ySize << "\tzSize\t:\t" << zSize << endl;
    cout << "xNumOfGrid\t:\t" << xNumOfGrid << "\yNumOfGrid\t:\t" << yNumOfGrid << "\tzNumOfGrid\t:\t" << zNumOfGrid << endl;
    if (theGrid)
    {
        cout << "theGrid : OK" << endl;
        cout << "Printing out theGrid below: " << endl;
        for (int i = 0; i<xNumOfGrid; i++)
        {
            for (int j = 0; j<zNumOfGrid; j++)
            {
                for (int k = 0; k < yNumOfGrid; k++)
                {
                    theGrid[(i*zNumOfGrid + j) + (k*xNumOfGrid*zNumOfGrid)].PrintSelf();
                }
            }
        }
    }
    else
        cout << "theGrid : NULL" << endl;
    cout << "******* End of CSpatialPartition::PrintSelf() **********************************" << endl;
}
