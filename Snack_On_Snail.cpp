
/*
"Ah, behold! A creation of immense power, resting here in its domain.
Now, heed this warning: Do not dare tamper with this
unless you are certain of your skill, for the path ahead is fraught with peril.

The creator, wise though they may be, suggests you keep backups of the old code,
should your tinkering lead to unforeseen calamities.
Learn from past mistakes, and do not repeat them, for this is not the time for rashness.

Now, the tale of this creation is not one told in mere words.
No, this is the language of the machine itself, a thing of great complexity and subtlety.

If you find yourself stumbling along this path, fear not.
You may mark the causes of your troubles, as many have before you.

---THE LIST OF WOE AND WONDER---

Time lost: 49 hours

"I've failed, and oh, how I’ve failed!" : 25 occasions

"By the gods, what have I done!" : 8 times

"Impossible... but it worked! Wonders never cease!" : 13 moments of triumph
*/

#include <GL/glut.h>
#include <soil.h> // Include SOIL for loading textures
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

// Global music variable
Mix_Music* backgroundMusic = nullptr;
Mix_Music* LobbyMusic = nullptr;
Mix_Chunk* collisionSound = nullptr;
Mix_Chunk* WinSound = nullptr;
Mix_Chunk* LoseSound = nullptr;

// Texturing
GLuint carTexture;  // Texture for the player's car
GLuint enemyTexture; // Texture for the enemy car
GLuint obstacleTexture; // Texture for obstacles
GLuint backgroundTexture; // Texture to store the background texture
GLuint winTexture; // Texture when you win
GLuint loseTexture; // Texture when you lose
GLuint mainMenuTexture; // For main menu - im starting to see pattern here

vector<GLuint> obstacleTextures; // To store obstacle textures


// Window dimensions
const int WIDTH = 800, HEIGHT = 1000;
//const float scaleY = 600.0f / HEIGHT; // Scaling factor based on the original 600 height

// Car and road properties
float playerCarX = 0.0f; // Player car's horizontal position (X axis)
float playerCarY = -0.8f;
float playerTargetX = 0.0f; // Planning to do some smooth transitions
float enemyCarX = 0.0f;  // Enemy car's horizontal position (X axis)
float enemyCarY = 0.5f;
float enemyTargetX = enemyCarX; //Again, another transition for enemy
float carSpeed = 1.0f;    // Speed of both cars and obstacles
float roadOffset = 0.0f;  // Offset of the road for movement
float carWidth = 0.2f, carHeight = 0.2f; // Dimensions of the cars
const float laneWidth = 0.4f; // Width of the lanes
vector<float> lanes = { -0.8f, -0.4f, 0.0f, 0.4f, 0.8f }; // Positions of the lanes

// Global variable for scrolling background
float backgroundOffset = 0.0f; // Initial offset
const float scrollSpeed = 0.00805f; // Scrolling speed (adjust as needed)

// Obstacles structure
struct Obstacle {
    float x, y, width, height; // Position and size of the obstacle
    GLuint texture;
};
vector<Obstacle> obstacles; // List of obstacles on the road
float spawnInterval = 1.5f; // Time interval between spawning obstacles
float timeSinceLastSpawn = 0.0f; // Time counter to track obstacle spawn timing
float timeElapsed = 0.0f;  // Track the time elapsed in seconds

// Speed properties for player and enemy
float playerSpeed = 0.5f; // Player's current Speed (affects speed and health)
float enemySpeed = 0.5f;  // Enemy's current Speed (affects speed and behavior)
float SpeedDecay = 0.005f; // How much Speed decays over time
float immunityFrames = 0.0f; // Immune from taking Speed loss from same obstacle
float enemyImmunityFrames = 0.0f; // Self explanatory...
// Global variable for volume
int volume = 50; // Start at maximum volume
int score = 10000;


// Game state
bool isGameOver = false; // Flag to track if the game is over
bool GameWon = false; //Flag to check is you actually catch the snail.
bool GameStarted = false;

// Check for collision between player car and enemy car
bool checkCollision(float playerX, float playerY, float enemyX, float enemyY) {
    return (abs(playerX - enemyX) < (carWidth) && abs(playerY - enemyY) < (carHeight));
}

// Make the background move
void updateBackgroundOffset() {
    backgroundOffset += scrollSpeed;
    if (backgroundOffset > 1.0f) {
        backgroundOffset = fmod(backgroundOffset, 1.0f); // Loop the offset
    }
}

// Load main menu
void loadMainMenuTexture() {
    // Load the main menu texture
    mainMenuTexture = SOIL_load_OGL_texture("Assets/1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (!mainMenuTexture) {
        cout << "Failed to load main menu texture!" << endl;
    }
}

// Where images were called
void loadTextures() {

    // Load win and lose textures
    winTexture = SOIL_load_OGL_texture("Assets/2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    loseTexture = SOIL_load_OGL_texture("Assets/3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    if (!winTexture || !loseTexture) {
        cout << "Failed to load win or lose texture!" << endl;
    }
    // Load background texture
    backgroundTexture = SOIL_load_OGL_texture(
        "Assets/bg.png", // Path to the background image
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    if (!backgroundTexture) {
        cout << "Failed to load background texture!" << endl;
    }

    // Load Player Texture
    carTexture = SOIL_load_OGL_texture(
        "Assets/NET.png", // Path to player texture
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    if (!carTexture) {
        cout << "Failed to load player texture!" << endl;
    }

    // Load Enemy Texture
    enemyTexture = SOIL_load_OGL_texture(
        "Assets/enemy.png", // Path to enemy texture
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    if (!enemyTexture) {
        cout << "Failed to load enemy texture!" << endl;
    }
    vector<string> textureFiles = {
        "Assets/Obst4.png",
        "Assets/Obst5.png",
        "Assets/Obst6.png",
        "Assets/Obst7.png",
        "Assets/Obst8.png",
        "Assets/Timmy.png",
    }; // Add your image file paths

    for (const string& file : textureFiles) {
        GLuint texture = SOIL_load_OGL_texture(
            file.c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y
        );

        if (texture == 0) {
            cout << "Failed to load texture: " << file << endl;
        }
        else {
            obstacleTextures.push_back(texture);
        }
    }
}

// Render resource bars
void renderSpeedBars() {
    // Define bar dimensions
    float barWidth = 0.4f;   // Maximum width of each bar
    float barHeight = 0.05f; // Height of each bar
    float containerThickness = 0.03f; // Thickness of the container border

    // Common y-coordinate for both bars
    float barY = 0.9f;
    float barYsound = -0.9f;

    // Positions for the player and enemy bars
    float playerBarX = -0.9f;          // Left side for the player
    float enemyBarX = 0.5f;            // Right side for the enemy

    // Calculate bar lengths based on speed
    float playerBarLength = playerSpeed * barWidth;
    float enemyBarLength = enemySpeed * barWidth;

    // Map the volume (which is an int) to a float in the range [0.0, 1.0] and then scale it by barWidth
    float soundlength = (static_cast<float>(volume) / 100.0f) * barWidth;

    // Draw the player's bar container (outline)
    glColor3f(0.0f, 0.0f, 0.0f); // Black for the container
    glBegin(GL_QUADS);
    glVertex2f(playerBarX - containerThickness, barY - containerThickness);
    glVertex2f(playerBarX + barWidth + containerThickness, barY - containerThickness);
    glVertex2f(playerBarX + barWidth + containerThickness, barY + barHeight + containerThickness);
    glVertex2f(playerBarX - containerThickness, barY + barHeight + containerThickness);
    glEnd();

    // Draw the player's speed bar (inside the container)
    glColor3f(0.0f, 1.0f, 0.0f); // Green for player
    glBegin(GL_QUADS);
    glVertex2f(playerBarX, barY);
    glVertex2f(playerBarX + playerBarLength, barY);
    glVertex2f(playerBarX + playerBarLength, barY + barHeight);
    glVertex2f(playerBarX, barY + barHeight);
    glEnd();

    // Draw the enemy's bar container (outline)
    glColor3f(0.0f, 0.0f, 0.0f); // Black for the container
    glBegin(GL_QUADS);
    glVertex2f(enemyBarX - containerThickness, barY - containerThickness);
    glVertex2f(enemyBarX + barWidth + containerThickness, barY - containerThickness);
    glVertex2f(enemyBarX + barWidth + containerThickness, barY + barHeight + containerThickness);
    glVertex2f(enemyBarX - containerThickness, barY + barHeight + containerThickness);
    glEnd();

    // Draw the enemy's speed bar (inside the container)
    glColor3f(1.0f, 0.0f, 0.0f); // Red for enemy
    glBegin(GL_QUADS);
    glVertex2f(enemyBarX, barY);
    glVertex2f(enemyBarX + enemyBarLength, barY);
    glVertex2f(enemyBarX + enemyBarLength, barY + barHeight);
    glVertex2f(enemyBarX, barY + barHeight);
    glEnd();

    // Draw the sound container (outline)
    glColor3f(0.0f, 0.0f, 0.0f); // Black for the container
    glBegin(GL_QUADS);
    glVertex2f(enemyBarX - containerThickness, barYsound - containerThickness);
    glVertex2f(enemyBarX + barWidth + containerThickness, barYsound - containerThickness);
    glVertex2f(enemyBarX + barWidth + containerThickness, barYsound + barHeight - 0.03f + containerThickness);
    glVertex2f(enemyBarX - containerThickness, barYsound + barHeight - 0.03f + containerThickness);
    glEnd();

    // Draw the sound volume bar (inside the container)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow for sound volume bar
    glBegin(GL_QUADS);
    glVertex2f(enemyBarX, barYsound);
    glVertex2f(enemyBarX + soundlength, barYsound); // Use the mapped soundlength
    glVertex2f(enemyBarX + soundlength, barYsound + barHeight- 0.03f);
    glVertex2f(enemyBarX, barYsound + barHeight - 0.03f);
    glEnd();

    // Reset color to white to avoid affecting subsequent drawings
    glColor3f(1.0f, 1.0f, 1.0f);
}

// Function to display text with scaling
void drawText(float x, float y, string text, float scaleX = 5.0f, float scaleY = 5.0f, float offset = 0.002f) {
    glPixelZoom(scaleX, scaleY); // Scale the text
    for (float dx = -offset; dx <= offset; dx += offset) {
        for (float dy = -offset; dy <= offset; dy += offset) {
            glRasterPos2f(x + dx, y + dy); // Adjust position for bold layers
            for (char c : text) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
            }
        }
    }
    glPixelZoom(1.0f, 1.0f); // Reset scaling
}

// Random number generator
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min))); // Generates a random float between min and max
}

// Draw a rectangle (used for cars and obstacles)
void drawTexturedRect(float x, float y, float width, float height, GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(x - width / 2, y - height / 2); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width / 2, y - height / 2); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex2f(x + width / 2, y + height / 2); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex2f(x - width / 2, y + height / 2); // Top-left
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Spawn obstacles in random lanes (maximum of 3 obstacles at a time)
void spawnObstacle() {
    if (obstacleTextures.empty()) {
        cout << "Error: No textures available to spawn obstacles." << endl;
        return; // Prevent further execution
    }

    vector<int> usedLanes;
    int lanesToFill = rand() % 3 + 1;
    for (int i = 0; i < lanesToFill; ++i) {
        int laneIndex;
        do {
            laneIndex = rand() % lanes.size();
        } while (find(usedLanes.begin(), usedLanes.end(), laneIndex) != usedLanes.end());

        usedLanes.push_back(laneIndex);
        Obstacle obs = {
            lanes[laneIndex],
            1.0f,
            0.2f,
            0.1f,
            obstacleTextures[rand() % obstacleTextures.size()] // Random texture
        };
        obstacles.push_back(obs);
    }
}

// Update obstacles and handle collision
void updateObstacles(float deltaTime) {
    for (auto& obs : obstacles) {  // Loop through each obstacle
        obs.y -= carSpeed * deltaTime; // Move the obstacle downward
    }
    obstacles.erase(remove_if(obstacles.begin(), obstacles.end(), [](Obstacle& obs) {
        return obs.y < -1.0f; // Remove obstacles that go off the screen
        }),
        obstacles.end());

    // Handle immunity frames for player (avoid multiple collisions)
    if (immunityFrames > 0.0f) {
        immunityFrames -= deltaTime; // Reduce immunity frames over time
    }

    // Player collision detection
    if (immunityFrames <= 0.0f) {
        for (const auto& obs : obstacles) { // Loop through obstacles
            if (abs(playerCarX - obs.x) < (carWidth + obs.width) / 2 && // Check horizontal collision
                abs(playerCarY - obs.y) < (carHeight + obs.height) / 2) { // Check vertical collision
                playerSpeed = max(0.0f, playerSpeed - 0.25f); // Reduce player Speed on collision
                immunityFrames = 1.0f; // Give player immunity for 1 second
                // Play the collision sound
                Mix_PlayChannel(-1, collisionSound, 0);
                if (playerSpeed <= 0.0f) {  // Game over if Speed reaches 0
                    // Play the collision sound
                    Mix_HaltMusic(); // Stop background music
                    Mix_PlayChannel(-1, LoseSound, 0);
                    isGameOver = true;
                }
            }
        }
    }

    // Enemy collision detection with immunity frames
    if (enemyImmunityFrames <= 0.0f) { // Only check if enemy is not immune
        for (const auto& obs : obstacles) { // Loop through obstacles for enemy
            if (abs(enemyCarX - obs.x) < (carWidth + obs.width) / 2 &&
                abs(obs.y - 0.8f) < carHeight / 2) { // Check horizontal and vertical collision for enemy
                enemySpeed = max(0.0f, enemySpeed * 0.75f); // Reduce enemy Speed on collision
                enemyImmunityFrames = 1.0f; // Give enemy immunity for 1 second
                // Play the collision sound
                Mix_PlayChannel(-1, collisionSound, 0);
            }
        }
    }
}
 
// Update the enemy's logic (now with immunity frames)
void updateEnemy(float deltaTime) {
    // First, check for collisions before applying Speed decay
    if (enemyImmunityFrames <= 0.0f) { // Only check if enemy is not immune
        bool collided = false;
        for (const auto& obs : obstacles) { // Loop through obstacles for enemy
            if (abs(enemyCarX - obs.x) < (carWidth + obs.width) / 2 && // Check horizontal collision
                abs(obs.y - 0.8f) < carHeight / 2) { // Check vertical collision
                collided = true;
                break;
            }
        }

        // If the enemy collided with any obstacle, reduce Speed
        if (collided) {
            enemySpeed = max(0.0f, enemySpeed * 0.75f); // Reduce enemy Speed on collision
            enemyImmunityFrames = 1.0f; // Set immunity frames
        }
    }

    // Apply Speed decay
    if (enemySpeed > 0.0f) {
        enemySpeed -= SpeedDecay * deltaTime; // Reduce Speed over time
    }

    // Smooth transition logic for enemy
    float smoothingFactor = 5.0f; // Control the speed of sliding
    enemyCarX += (enemyTargetX - enemyCarX) * deltaTime * smoothingFactor; // Smooth transition

    bool isDangerDetected = false;
    // First, check if danger is near
    for (const auto& obs : obstacles) {
        if (abs(enemyCarX - obs.x) < (carWidth + obs.width) / 2 && obs.y > 0.8f) { // Danger detected
            isDangerDetected = true;
            break;
        }
    }

    // If danger is detected, try to find the safest lane
    if (isDangerDetected) {
        vector<float> safeLanes;

        // Check all lanes for safety
        for (float lane : lanes) {
            bool isSafe = true;
            for (const auto& otherObs : obstacles) { // Ensure no obstacles block the lane
                if (abs(lane - otherObs.x) < (carWidth + otherObs.width) / 2 && otherObs.y > 0.8f) {
                    isSafe = false;
                    break;
                }
            }

            if (isSafe) {
                safeLanes.push_back(lane); // Add to safe lanes if no obstacles
            }
        }

        // If we have safe lanes, pick one. Otherwise, stay in the current lane
        if (!safeLanes.empty()) {
            // Randomly choose a safe lane from the list
            enemyTargetX = safeLanes[rand() % safeLanes.size()];
        }
    }

    // Reduce enemy immunity frames over time
    if (enemyImmunityFrames > 0.0f) {
        enemyImmunityFrames -= deltaTime;
    }
}

// Render the lose screen after game over
void renderWinScreen() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen
    drawTexturedRect(0.0f, 0.0f, 2.0f, 2.0f, winTexture); // Display the win screen image
    // Render the score text (adjusted position and scaling)
    drawText(-0.2f, 0.5f, "Score: " + to_string(static_cast<int>(score)), 0.0f, 0.5f);   // 2x bigger and bold
    glutSwapBuffers(); // Swap the buffer to display the win screen
}

// Render the lose screen after game over
void renderLoseScreen() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen first
    // Display the lose screen image
    drawTexturedRect(0.0f, 0.0f, 2.0f, 2.0f, loseTexture);
    glutSwapBuffers(); // Swap the buffer to display the lose screen
    
}

//Update game in real time
void updateGame(float deltaTime) {
    if (isGameOver) return; // Skip the game update if game is over
    if (GameWon) return; // Skip the game update if you win the game
    if (!GameStarted) return;

    if (GameStarted == true) {
        // Deduct score over time
        timeElapsed += deltaTime;
        score -= 1;  // Decrease score every frame (tweak value as needed)

        if (score < 0) {
            score = 0;  // Prevent score from going negative
        }
    }

    float smoothingFactor = 5.0f;
    playerCarX += (playerTargetX - playerCarX) * deltaTime * smoothingFactor;

    roadOffset -= carSpeed * deltaTime; // Update road movement
    timeSinceLastSpawn += deltaTime; // Increase the spawn timer

    playerSpeed = min(1.0f, playerSpeed + deltaTime * 0.05f); // Gradually increase player Speed
    enemySpeed = min(1.0f, enemySpeed + deltaTime * 0.15f); // Gradually increase enemy Speed

    // Smooth transition for playerCarY based on Speed comparison
    float ySmoothingFactor = 0.2f;  // Factor to control the speed of Y-position adjustment
    if (playerSpeed >= enemySpeed) {
        // If the player's Speed is equal to or greater, move towards enemy's Y position continuously
        playerCarY += (enemyCarY - playerCarY) * ySmoothingFactor * deltaTime;
    }
    else {
        // If the player's Speed is less than the enemy's, return to the initial Y position
        playerCarY += (-0.8f - playerCarY) * ySmoothingFactor * deltaTime;
    }

    // Check for collision between player and enemy

    if (checkCollision(playerCarX, playerCarY, enemyCarX, enemyCarY)) {
        playerCarY = -0.8f;
        cout << "game won";
        GameWon = true;  // Set game over if there's a collision
        // Play the collision sound
        Mix_HaltMusic(); // Stop background music
        Mix_PlayChannel(-1, WinSound, 0);
    }

    if (timeSinceLastSpawn > spawnInterval) {  // If enough time has passed, spawn an obstacle
        spawnObstacle();
        timeSinceLastSpawn = 0.0f; // Reset spawn timer
    }

    updateObstacles(deltaTime); // Update obstacle positions and check for collisions
    updateEnemy(deltaTime); // Update enemy car logic (dodge obstacles)
}

// Render the game screend
void renderGame() {
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f); // Set background color to cyan
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen

    if (!GameStarted) {
        // Display main menu texture
        glBindTexture(GL_TEXTURE_2D, mainMenuTexture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);  // Bottom-left corner
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);  // Bottom-right corner
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);  // Top-right corner
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);  // Top-left corner
        glEnd();
    }
    else { 
        // Update the background offset
        updateBackgroundOffset();

        // Enable texture mapping
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        // Ensure texture wrapping is set to repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Draw the scrolling background
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f + backgroundOffset); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f + backgroundOffset); glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f + backgroundOffset); glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f + backgroundOffset); glVertex2f(-1.0f, 1.0f);
        glEnd();

        // Draw the player car with its texture
        drawTexturedRect(playerCarX, playerCarY, carWidth, carHeight, carTexture);

        // Draw the enemy car with its texture
        drawTexturedRect(enemyCarX, enemyCarY, carWidth, carHeight + 0.1f, enemyTexture);

        // Draw the obstacles with random textures
        for (const auto& obs : obstacles) {
            drawTexturedRect(obs.x, obs.y, obs.width + 0.1f, obs.height + 0.1f, obs.texture);
        }
        renderSpeedBars();
        // Display the player and enemy Speed on the screen with scaling and bold effect
        drawText(-0.9f, 0.8f, "Player Speed: " + to_string(static_cast<int>(playerSpeed * 100)), 2.5f, 0.5f); // 1.5x bigger and bold
        drawText(0.5f, 0.8f, "Snail Speed: " + to_string(static_cast<int>(enemySpeed * 100)), 2.5f, 0.5f);   // 2x bigger and bold
        drawText(-0.9f, 0.7f, "Efficiency: " + to_string(static_cast<int>(score)), 8.5f, 0.5f);   // 2x bigger and bold
        drawText(0.5f, -0.8f, "Volume: " + to_string(static_cast<int>(volume)), 8.5f, 0.5f);
    }

    glutSwapBuffers(); // Swap the buffer to display the game screen
}

// Keyboard input handler
void handleKeys(unsigned char key, int x, int y) {
    if (isGameOver || GameWon) { // If the game is over
        if (key == 'r' || key == 'R') { // Retry if 'r' or 'R' is pressed
            isGameOver = false;
            GameWon = false;
            GameStarted = true;
            playerSpeed = 0.5f; // Reset player Speed
            obstacles.clear(); // Clear obstacles
            Mix_HaltMusic(); // Stop background music
            Mix_HaltChannel(-1);
            
            if (Mix_PlayingMusic() == 0) {
                Mix_PlayMusic(backgroundMusic, -1); // Play in loop
            }
        }
        else if (key == 'q' || key == 'Q') { // Quit if 'q' or 'Q' is pressed
            Mix_HaltMusic(); // Stop background music
            Mix_HaltChannel(-1);
            exit(0); // Exit the game
        }
        return;
    }

    if (!GameStarted) {
        if (key == ' ') {
            Mix_HaltMusic(); // Stop background music
            if (Mix_PlayingMusic() == 0) {
                Mix_PlayMusic(backgroundMusic, -1); // Play in loop
            }
            // Start game when space is pressed
            GameStarted = true;
            cout << GameStarted;
        }
        else if (key == 'q') {
            exit(0); // Exit game
        }
    }

    // Determine the nearest lane index
    size_t nearestLane = 0;
    float minDistance = fabs(playerCarX - lanes[0]);
    for (size_t i = 1; i < lanes.size(); ++i) {
        float distance = fabs(playerCarX - lanes[i]);
        if (distance < minDistance) {
            nearestLane = i;
            minDistance = distance;
        }
    }

    // Active controls during game
    // Move left or right
    if (key == 'a' || key == 'A') { // Move left
        if (nearestLane > 0) {
            playerTargetX = lanes[nearestLane - 1];
        }
    }
    else if (key == 'd' || key == 'D') { // Move right
        if (nearestLane < lanes.size() - 1) {
            playerTargetX = lanes[nearestLane + 1];
        }
    }

    // Volume controls
    if (key == '+' || key == '=') { // Increase volume
        volume = min(volume + 10, 100); // Increase by 8, capped at MIX_MAX_VOLUME
        Mix_VolumeMusic(volume);
        // Adjust the volume of each sound effect individually
        Mix_VolumeChunk(WinSound, volume); // Set win sound volume
        Mix_VolumeChunk(LoseSound, volume); // Set lose sound volume
        cout << "Volume increased: " << volume << endl;
    }
    else if (key == '-' || key == '_') { // Decrease volume
        volume = max(volume - 10, 0); // Decrease by 8, minimum 0
        Mix_VolumeMusic(volume);
        // Adjust the volume of each sound effect individually
        Mix_VolumeChunk(WinSound, volume); // Set win sound volume
        Mix_VolumeChunk(LoseSound, volume); // Set lose sound volume
        cout << "Volume decreased: " << volume << endl;
    }
    cout << volume << endl;
}

// Timer function for updating the game logic
void timer(int value) {
    float deltaTime = 0.016f;  // Fixed delta time (60 FPS)
    updateGame(0.016f);  // Update the game with a fixed delta time (60 FPS)
    if (isGameOver) {
        renderLoseScreen();  // Render the lose screen if game is over
    }
    else if (GameWon) {
        renderWinScreen();
    }
    else {
        renderGame();  // Otherwise, render the game screen
    }
    glutTimerFunc(16, timer, 0); // Call timer again in 16 ms (about 60 FPS)
}

// Main function 
int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(0)));  // Seed for random number generation
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Car Game");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return -1;
    }

    // Load the background music
    backgroundMusic = Mix_LoadMUS("Assets/BgSong.ogg");
    if (!backgroundMusic) {
        cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
    }
    // Load the background music
    LobbyMusic = Mix_LoadMUS("Assets/Lobby.ogg");
    if (!backgroundMusic) {
        cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
    }
    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(LobbyMusic, -1); // Play in loop
    }
    // Load the collision sound
    collisionSound = Mix_LoadWAV("Assets/hit.ogg");
    if (!collisionSound) {
        cout << "Failed to load collision sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
    }
    // Load the collision sound
    WinSound = Mix_LoadWAV("Assets/WinGame.ogg");
    if (!collisionSound) {
        cout << "Failed to load collision sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
    }
    // Load the collision sound
    LoseSound = Mix_LoadWAV("Assets/LoseGame.ogg");
    if (!collisionSound) {
        cout << "Failed to load collision sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
    }

    // OpenGL setup
    glEnable(GL_BLEND);                  // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Set blend mode

    glEnable(GL_TEXTURE_2D); // Enable 2D texturing
    loadTextures();  // Load all textures
    loadMainMenuTexture();  // Load the main menu texture

    // Setup GLUT callbacks
    glutDisplayFunc(renderGame);
    glutKeyboardFunc(handleKeys);
    glutTimerFunc(16, timer, 0);  // Timer for game updates (60 FPS)


    glutMainLoop();  // Start the main loop
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
    Mix_CloseAudio();
    return 0;
}
