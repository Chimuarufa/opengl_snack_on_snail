//#include <GL/glut.h>
//#include <iostream>
//#include <cmath>
//#include <cstdlib>  // For random number generation
//#include <ctime>    // For seeding the random generator
//#include <soil.h>
//
//// Window dimensions
//const int WIDTH = 800, HEIGHT = 600;
//
//// Player state
//struct Player {
//    float x = 0.0f, y = -0.8f; // Initialize position
//    float speed = 0.1f;       // Default speed
//    bool enraged = false;     // Default state
//    float enrageTimer = 0.0f; // Default timer value
//    bool slowed = false;      // Default state
//    float slowTimer = 0.0f;   // Default timer value
//} player;
//
//// Texture ID for player
//GLuint playerTexture;
//
//struct Object {
//    float x; // Horizontal position
//    float y; // Vertical position
//    bool active; // Is the object active (on-screen)?
//};
//
//// Arrays for obstacles and power-ups
//const int maxObstacles = 5;
//const int maxPowerUps = 3;
//Object obstacles[maxObstacles];
//Object powerUps[maxPowerUps];
//
//// Obstacles and power-ups
//float obstacleX = 0.5f;
//float powerUpX = 1.5f;
//float obstacleY = 1.0f;  // Start at the top of the screen
//float powerUpY = 2.0f;   // Start higher than obstacles
//float trackOffset = 0.0f; // Tracks the vertical offset for simulating track movement
//
//// Timing
//float deltaTime = 0.0f;
//int lastTime = 0;
//float distanceTravelled = 0.0f;
//
//
//bool checkCollision(float ax, float ay, float aw, float ah,
//    float bx, float by, float bw, float bh) {
//    return (ax < bx + bw && ax + aw > bx &&
//        ay < by + bh && ay + ah > by);
//}
//
//
//// Initialize random positions
//void initObjects() {
//    srand(static_cast<unsigned>(time(0))); // Seed the random number generator
//
//    for (int i = 0; i < maxObstacles; ++i) {
//        obstacles[i] = {
//            static_cast<float>(rand() % 100 / 100.0f) - 0.5f, // Random x within road bounds
//            static_cast<float>(rand() % 200 / 100.0f) + 1.0f, // Random y above the screen
//            true
//        };
//    }
//
//    for (int i = 0; i < maxPowerUps; ++i) {
//        powerUps[i] = {
//            static_cast<float>(rand() % 100 / 100.0f) - 0.5f, // Random x within road bounds
//            static_cast<float>(rand() % 200 / 100.0f) + 1.0f, // Random y above the screen
//            true
//        };
//    }
//}
//
//void loadTexture() {
//    playerTexture = SOIL_load_OGL_texture("Asset/Obs1.png",
//        SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
//    if (playerTexture == 0) {
//        std::cerr << "Texture loading failed: " << SOIL_last_result() << std::endl;
//
//    }
//    else {
//        glBindTexture(GL_TEXTURE_2D, playerTexture);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//        std::cout << "Texture loaded successfully." << std::endl;
//    }
//}
//
//void init() {
//    glEnable(GL_TEXTURE_2D);
//
//    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Background color
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Set orthographic projection
//    // Load the player texture
//    loadTexture(); // Replace with your texture file path
//}
//
//// Load texture using SOIL2
//
//void updateObjects() {
//    for (int i = 0; i < maxObstacles; ++i) {
//        if (obstacles[i].active) {
//            // Move the obstacle
//            obstacles[i].y -= (player.enraged ? player.speed * 1.5f : player.speed) * deltaTime;
//
//            // Respawn obstacle if it goes off-screen
//            if (obstacles[i].y < -1.0f) {
//                obstacles[i].x = static_cast<float>(rand() % 100 / 100.0f) - 0.5f;
//                obstacles[i].y = static_cast<float>(rand() % 200 / 100.0f) + 1.0f;
//            }
//
//            // Check collision with player
//            if (checkCollision(player.x - 0.05f, player.y - 0.05f, 0.1f, 0.1f, // Player bounding box
//                obstacles[i].x - 0.05f, obstacles[i].y - 0.05f, 0.1f, 0.1f)) {
//                if (!player.enraged) {
//                    player.slowed = true;
//                    player.slowTimer = 2.0f; // Slowdown lasts for 2 seconds
//                    player.speed *= 0.7f;   // Reduce speed by 30%
//                }
//                obstacles[i].active = false; // Deactivate obstacle after collision
//            }
//        }
//    }
//
//    for (int i = 0; i < maxPowerUps; ++i) {
//        if (powerUps[i].active) {
//            // Move the power-up
//            powerUps[i].y -= player.speed * deltaTime;
//
//            // Respawn power-up if it goes off-screen
//            if (powerUps[i].y < -1.0f) {
//                powerUps[i].x = static_cast<float>(rand() % 100 / 100.0f) - 0.5f;
//                powerUps[i].y = static_cast<float>(rand() % 200 / 100.0f) + 1.0f;
//            }
//
//            // Check collision with player
//            if (checkCollision(player.x - 0.05f, player.y - 0.05f, 0.1f, 0.1f, // Player bounding box
//                powerUps[i].x - 0.05f, powerUps[i].y - 0.05f, 0.1f, 0.1f)) {
//                player.enraged = true;          // Activate "enrage" state
//                player.enrageTimer = 5.0f;     // Enrage lasts 5 seconds
//                player.speed *= 1.5f;          // Boost speed by 50%
//                powerUps[i].active = false;    // Deactivate power-up after collection
//            }
//        }
//    }
//
//    // Update slowdown timer
//    if (player.slowed) {
//        player.slowTimer -= deltaTime;
//        if (player.slowTimer <= 0) {
//            player.slowed = false;
//            player.speed /= 0.7f; // Reset speed to normal
//        }
//    }
//
//    // Update enrage timer
//    if (player.enraged) {
//        player.enrageTimer -= deltaTime;
//        if (player.enrageTimer <= 0) {
//            player.enraged = false;
//            player.speed /= 1.5f; // Reset speed to normal
//        }
//    }
//}
//
//void updateTrack() {
//    // Adjust trackOffset based on the player's current speed
//    trackOffset -= (player.enraged ? player.speed * 1.5f : player.speed) * deltaTime;
//
//    // Reset trackOffset to create a looping effect
//    if (trackOffset < -1.0f) {
//        trackOffset += 1.0f; // Reset when it scrolls off-screen
//    }
//}
//
//void update() {
//    updateTrack();
//    updateObjects();
//    int currentTime = glutGet(GLUT_ELAPSED_TIME);
//    deltaTime = (currentTime - lastTime) / 1000.0f;
//    lastTime = currentTime;
//
//    // Track progress
//    distanceTravelled += player.speed * deltaTime;
//
//    // Check if player reaches the finish line
//    if (distanceTravelled >= 10.0f) { // Example: 10 "units" to finish
//        std::cout << "You win!" << std::endl;
//        exit(0); // End game
//    }
//
//    // Move track (obstacles and power-ups)
//    obstacleY -= player.speed * deltaTime;
//    powerUpY -= player.speed * deltaTime;
//
//    // Reset obstacles and power-ups when they go off-screen
//    if (obstacleY < -1.0f) obstacleY = 1.0f; // Reset to top of the screen
//    if (powerUpY < -1.0f) powerUpY = 1.5f;
//
//    // Collision detection
//    if (fabs(player.x - obstacleX) < 0.1f && fabs(player.y - obstacleY) < 0.1f && !player.enraged) {
//        player.speed = 0.005f; // Slow down on collision
//    }
//
//    if (fabs(player.x - powerUpX) < 0.1f && fabs(player.y - powerUpY) < 0.1f) {
//        player.enraged = true;
//        player.enrageTimer = 5.0f; // Enrage lasts 5 seconds
//    }
//
//    // Handle enrage state
//    if (player.enraged) {
//        player.speed = 0.15f; // Increased speed
//        player.enrageTimer -= deltaTime;
//        if (player.enrageTimer <= 0.0f) {
//            player.enraged = false;
//            player.speed = 0.01f; // Reset speed
//        }
//    }
//
//    // Redraw the scene
//    glutPostRedisplay();
//}
//
//// Draw the player with texture
//void drawPlayer(float x, float y, float width, float height, GLuint texture) {
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, texture);
//
//    glColor3f(1.0f, 1.0f, 1.0f); // Ensure color is white for proper texturing
//    glBegin(GL_QUADS);
//    glTexCoord2f(0.0f, 0.0f); glVertex2f(x - width / 2, y - height / 2);
//    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width / 2, y - height / 2);
//    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width / 2, y + height / 2);
//    glTexCoord2f(0.0f, 1.0f); glVertex2f(x - width / 2, y + height / 2);
//    glEnd();
//
//    glDisable(GL_TEXTURE_2D);
//}
//
//
//void drawObstacles() {
//    glColor3f(1.0f, 0.0f, 0.0f); // Red for obstacles
//    for (int i = 0; i < maxObstacles; ++i) {
//        if (obstacles[i].active) {
//            glBegin(GL_QUADS);
//            glVertex2f(obstacles[i].x - 0.05f, obstacles[i].y - 0.05f);
//            glVertex2f(obstacles[i].x + 0.05f, obstacles[i].y - 0.05f);
//            glVertex2f(obstacles[i].x + 0.05f, obstacles[i].y + 0.05f);
//            glVertex2f(obstacles[i].x - 0.05f, obstacles[i].y + 0.05f);
//            glEnd();
//        }
//    }
//}
//
//void drawPowerUps() {
//    glColor3f(0.0f, 0.0f, 1.0f); // Blue for power-ups
//    for (int i = 0; i < maxPowerUps; ++i) {
//        if (powerUps[i].active) {
//            glBegin(GL_QUADS);
//            glVertex2f(powerUps[i].x - 0.05f, powerUps[i].y - 0.05f);
//            glVertex2f(powerUps[i].x + 0.05f, powerUps[i].y - 0.05f);
//            glVertex2f(powerUps[i].x + 0.05f, powerUps[i].y + 0.05f);
//            glVertex2f(powerUps[i].x - 0.05f, powerUps[i].y + 0.05f);
//            glEnd();
//        }
//    }
//}
//
//void drawTrack() {
//    // Draw two sections of the track to create a seamless scrolling effect
//    glColor3f(0.3f, 0.3f, 0.3f); // Dark gray for the track
//    glBegin(GL_QUADS);
//    glVertex2f(-0.5f, -1.0f + trackOffset);
//    glVertex2f(0.5f, -1.0f + trackOffset);
//    glVertex2f(0.5f, 1.0f + trackOffset);
//    glVertex2f(-0.5f, 1.0f + trackOffset);
//    glEnd();
//
//    glBegin(GL_QUADS);
//    glVertex2f(-0.5f, -1.0f + trackOffset + 2.0f);
//    glVertex2f(0.5f, -1.0f + trackOffset + 2.0f);
//    glVertex2f(0.5f, 1.0f + trackOffset + 2.0f);
//    glVertex2f(-0.5f, 1.0f + trackOffset + 2.0f);
//    glEnd();
//}
//
//void display() {
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    drawTrack();
//    drawObstacles();
//    drawPowerUps();
//    drawPlayer(player.x, player.y, 0.2f, 0.2f, playerTexture);
//
//    glutSwapBuffers();
//}
//
//void handleInput(unsigned char key, int x, int y) {
//    float movementStep = 0.05f; // Adjust step size as needed
//
//    switch (key) {
//    case 'a': // Move left
//        if (player.x > -1.0f + 0.1f) // Ensure within bounds
//            player.x -= movementStep;
//        break;
//    case 'd': // Move right
//        if (player.x < 1.0f - 0.1f) // Ensure within bounds
//            player.x += movementStep;
//        break;
//    }
//    glutPostRedisplay(); // Redraw screen after input
//}
//
//
//int main(int argc, char** argv) {
//    // Initialize GLUT
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//    glutInitWindowSize(WIDTH, HEIGHT);
//    glutCreateWindow("Elephant Rush");
//
//    // Initialize OpenGL settings
//    glEnable(GL_BLEND);                  // Enable blending
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Set blend mode
//
//    glEnable(GL_TEXTURE_2D); // Enable 2D texturing
//
//    init();
//    initObjects();
//    loadTexture();
//
//    // Register callbacks
//    glutDisplayFunc(display);
//    glutIdleFunc(update);
//    glutKeyboardFunc(handleInput);
//
//    // Enter main loop
//    glutMainLoop();
//    return 0;
//}