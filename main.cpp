#include <stdlib.h>
#include <GL/glut.h>
#include <list>
#include <iostream>

#include <cstdlib>
#include <ctime>
#include <chrono>
#include <vector>

#include "globals.h"
#include "Vec3.h"
#include "ColliderObject.h"
#include "Box.h"
#include "MemoryFooter.h"
#include "MemoryHeader.h"
#include "MemoryTracker.h"
#include "Sphere.h"


using namespace std::chrono;

// This is the number of falling physical items. 
#define NUMBER_OF_BOXES 5
#define NUMBER_OF_SPHERES 5

#define RESOLUTION_X 1920
#define RESOLUTION_Y 1080

// This is where the camera is, where it is looking and the bounds of the containing box. You shouldn't need to alter these

#define LOOKAT_X 10
#define LOOKAT_Y 10
#define LOOKAT_Z 50

#define LOOKDIR_X 10
#define LOOKDIR_Y 0
#define LOOKDIR_Z 0

std::list<ColliderObject*> Colliders;

void CreateBox()
{
    Box* box = new Box();

    // Assign random x, y, and z positions within specified ranges
    box->Position.X = static_cast<float>(rand()) / (RAND_MAX / 20.0f);
    box->Position.Y = 10.0f + static_cast<float>(rand()) / (RAND_MAX / 1.0f);
    box->Position.Z = static_cast<float>(rand()) / (RAND_MAX / 20.0f);

    box->Size = { 1.0f, 1.0f, 1.0f };

    // Assign random x-velocity between -1.0f and 1.0f
    float randomXVelocity = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
    box->Velocity = { randomXVelocity, 0.0f, 0.0f };

    // Assign a random color to the box
    box->Colour.X = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    box->Colour.Y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    box->Colour.Z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    Colliders.push_back(box);
}

void CreateSphere()
{
    Sphere* sphere = new Sphere();

    // Assign random x, y, and z positions within specified ranges
    sphere->Position.X = static_cast<float>(rand()) / (RAND_MAX / 20.0f);
    sphere->Position.Y = 10.0f + static_cast<float>(rand()) / (RAND_MAX / 1.0f);
    sphere->Position.Z = static_cast<float>(rand()) / (RAND_MAX / 20.0f);

    sphere->Size = { 1.0f, 1.0f, 1.0f };

    // Assign random x-velocity between -1.0f and 1.0f
    float randomXVelocity = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
    sphere->Velocity = { randomXVelocity, 0.0f, 0.0f };

    // Assign a random color to the box
    sphere->Colour.X = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    sphere->Colour.Y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    sphere->Colour.Z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    Colliders.push_back(sphere);
}

void InitScene(int boxCount, int sphereCount)
{
    for (int i = 0; i < boxCount; ++i) {
        CreateBox();
    }

    for (int i = 0; i < sphereCount; ++i) {
        CreateSphere();
    }
}

// a ray which is used to tap (by default, remove) a box - see the 'mouse' function for how this is used.
bool RayBoxIntersection(const Vec3& rayOrigin, const Vec3& rayDirection, const ColliderObject* box)
{
    float tMin = (box->Position.X - box->Size.X / 2.0f - rayOrigin.X) / rayDirection.X;
    float tMax = (box->Position.X + box->Size.X / 2.0f - rayOrigin.X) / rayDirection.X;

    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (box->Position.Y - box->Size.Y / 2.0f - rayOrigin.Y) / rayDirection.Y;
    float tyMax = (box->Position.Y + box->Size.Y / 2.0f - rayOrigin.Y) / rayDirection.Y;

    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax))
        return false;

    if (tyMin > tMin)
        tMin = tyMin;

    if (tyMax < tMax)
        tMax = tyMax;

    float tzMin = (box->Position.Z - box->Size.Z / 2.0f - rayOrigin.Z) / rayDirection.Z;
    float tzMax = (box->Position.Z + box->Size.Z / 2.0f - rayOrigin.Z) / rayDirection.Z;

    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax))
        return false;

    return true;
}

// used in the 'mouse' tap function to convert a screen point to a point in the world
Vec3 ScreenToWorld(int x, int y)
{
    GLint viewport[4];
    GLdouble modelView[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    gluUnProject(winX, winY, winZ, modelView, projection, viewport, &posX, &posY, &posZ);

    return Vec3((float)posX, (float)posY, (float)posZ);
}

// update the physics: gravity, collision test, collision resolution
void UpdatePhysics(const float deltaTime)
{
    // TODO for the assessment - use a thread for each sub-region
    // for example, assuming we have two regions:
    // from 'colliders' create two separate lists
    // empty each list (from previous frame) and work out which collidable object is in which region, 
    //  and add the pointer to that region's list.
    // Then, run two threads with the code below (changing 'colliders' to be the region's list)

    for (ColliderObject* box : Colliders) { 
        
        box->Update(&Colliders, deltaTime);
        
    }
}

// draw the sides of the containing area
void DrawQuad(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& v4)
{
    glBegin(GL_QUADS);
    glVertex3f(v1.X, v1.Y, v1.Z);
    glVertex3f(v2.X, v2.Y, v2.Z);
    glVertex3f(v3.X, v3.Y, v3.Z);
    glVertex3f(v4.X, v4.Y, v4.Z);
    glEnd();
}



// draw the entire scene
void DrawScene()
{
    // Draw the side wall
    GLfloat diffuseMaterial[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);

    // Draw the left side wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 leftSideWallV1(MIN_X, 0.0f, MAX_Z);
    Vec3 leftSideWallV2(MIN_X, 50.0f, MAX_Z);
    Vec3 leftSideWallV3(MIN_X, 50.0f, MIN_Z);
    Vec3 leftSideWallV4(MIN_X, 0.0f, MIN_Z);
    DrawQuad(leftSideWallV1, leftSideWallV2, leftSideWallV3, leftSideWallV4);

    // Draw the right side wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 rightSideWallV1(MAX_X, 0.0f, MAX_Z);
    Vec3 rightSideWallV2(MAX_X, 50.0f, MAX_Z);
    Vec3 rightSideWallV3(MAX_X, 50.0f, MIN_Z);
    Vec3 rightSideWallV4(MAX_X, 0.0f, MIN_Z);
    DrawQuad(rightSideWallV1, rightSideWallV2, rightSideWallV3, rightSideWallV4);


    // Draw the back wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 backWallV1(MIN_X, 0.0f, MIN_Z);
    Vec3 backWallV2(MIN_X, 50.0f, MIN_Z);
    Vec3 backWallV3(MAX_X, 50.0f, MIN_Z);
    Vec3 backWallV4(MAX_X, 0.0f, MIN_Z);
    DrawQuad(backWallV1, backWallV2, backWallV3, backWallV4);

    for (ColliderObject* box : Colliders) {
        box->Draw();
    }
}

// called by GLUT - displays the scene
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(LOOKAT_X, LOOKAT_Y, LOOKAT_Z, LOOKDIR_X, LOOKDIR_Y, LOOKDIR_Z, 0, 1, 0);

    DrawScene();

    glutSwapBuffers();
}

// Called by GLUT when the cpu is idle - has a timer function you can use for FPS, and updates the physics
// see https://www.opengl.org/resources/libraries/glut/spec3/node63.html#:~:text=glutIdleFunc
// NOTE this may be capped at 60 fps as we are using glutPostRedisplay(). If you want it to go higher than this, maybe a thread will help here. 
void Idle()
{
    static auto last = steady_clock::now();
    auto old = last;
    last = steady_clock::now();
    const duration<float> frameTime = last - old;
    float deltaTime = frameTime.count();

    if (deltaTime > 1)
        return;

    UpdatePhysics(deltaTime);

    // tell glut to draw - note this will cap this function at 60 fps
    glutPostRedisplay();
}

// called the mouse button is tapped
void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Get the camera position and direction
        Vec3 cameraPosition(LOOKAT_X, LOOKAT_Y, LOOKAT_Z); // Replace with your actual camera position
        Vec3 cameraDirection(LOOKDIR_X, LOOKDIR_Y, LOOKDIR_Z); // Replace with your actual camera direction

        // Get the world coordinates of the clicked point
        Vec3 clickedWorldPos = ScreenToWorld(x, y);

        // Calculate the ray direction from the camera position to the clicked point
        Vec3 rayDirection = clickedWorldPos - cameraPosition;
        rayDirection.Normalise();

        // Perform a ray-box intersection test and remove the clicked box
        bool clickedBoxOK = false;
        float minIntersectionDistance = std::numeric_limits<float>::max();

        ColliderObject* intersectedBox = nullptr;
        for (ColliderObject* box : Colliders) {
            if (RayBoxIntersection(cameraPosition, rayDirection, box)) {
                // Calculate the distance between the camera and the intersected box
                Vec3 diff = box->Position - cameraPosition;

                // Update the clicked box index if this box is closer to the camera
                if (float distance = diff.Length(); distance < minIntersectionDistance) {
                    clickedBoxOK = true;
                    minIntersectionDistance = distance;
                    intersectedBox = box;
                }
            }
        }

        // Remove the clicked box if any
        if (clickedBoxOK != false) {
        	Colliders.remove(intersectedBox);
            delete intersectedBox;
        }
    }
}

void HeapChecker()
{
    std::cout << "Heap Walk Started\n";
    MemoryFooter* currentNode = MemoryHeader::Last;
    unsigned i = 0;

    while(currentNode)
    {
        if (currentNode->OverflowTest != OVERFLOW_TEST)
        {
	        std::cout << "Err: Overflow Test Failed" << '\n';
            return;
        }
        if (currentNode->Header->UnderflowTest != UNDERFLOW_TEST)
        {
            std::cout << "Err: Underflow Test Failed" << '\n';
            return;
        }

        std::cout << ++i << ". Size: " << currentNode->Header->Size << " bytes\n";
        currentNode = currentNode->Header->Previous;
    }

	std::cout << "Heap Walk Successful\n\n";
}

// called when the keyboard is used
void Keyboard(unsigned char key, int x, int y)
{
    static std::vector<int*> randomData{};
	if (key == ' ') // Spacebar key 
    { 
        for (ColliderObject* box : Colliders)
        {
	        constexpr float impulseMagnitude = 20.0f;
	        box->Velocity.Y += impulseMagnitude;
        }
    }
    else if (key == 'm')
    { 
        std::cout << "Memory used: " << MemoryTracker::Get().GetAllocation() << '\n';
        std::cout << "Box Memory used: " << Box::MemoryTracker.GetAllocation() << '\n';
        std::cout << "Sphere Memory Used: " << Sphere::MemoryTracker.GetAllocation() << '\n';
        std::cout << '\n';
    }
    else if (key == 't')
    {
        randomData.emplace_back(new int[10]);
    }
    else if (key == 'u')
    {
        if (!randomData.empty())
        {
            delete[] randomData.at(randomData.size() - 1);
	        randomData.erase(randomData.end() - 1);
            randomData.shrink_to_fit();
        }
    }
    else if (key == '2')
    {
	    for (ColliderObject* collider : Colliders)
	    {
            delete collider;
	    }

        Colliders.clear();
    }
    else if (key == 'w')
    {
        HeapChecker();
    }
}

// The main function. 
int main(int argc, char** argv)
{
    srand(static_cast<unsigned>(time(nullptr))); // Seed random number generator
    MemoryTracker::Get().RemoveAllocation(MemoryTracker::Get().GetAllocation()); // Removed random byte allocation before main even runs
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(RESOLUTION_X, RESOLUTION_Y);
    glutCreateWindow("Simple Physics Simulation");

    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(RESOLUTION_X) / RESOLUTION_Y, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    InitScene(NUMBER_OF_BOXES, NUMBER_OF_SPHERES);
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);

    // it will stick here until the program ends. 
    glutMainLoop();
    return 0;
}
