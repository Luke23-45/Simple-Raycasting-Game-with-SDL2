#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 20;
const float MOUSE_SENSITIVITY = 0.002f;

// Player settings
struct Player {
    float x = 5.0f;
    float y = 5.0f;
    float angle = 0.0f;
    float fov = M_PI / 3.0f;
};

// Enemy settings
struct Enemy {
    float x, y;
    int health;
    bool alive;
    float speed;
    Enemy(float x, float y) : x(x), y(y), health(100), alive(true), speed(0.5f) {}
};

// Map representation (0 = empty, 1 = wall)
int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,0,0,0,1,0,1,0,1,0,0,1,1,1,0,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,1},
    {1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,0,1,1,0,1,1,0,0,0,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,0,1,1,0,0,0,1,0,1,0,1,0,0,0,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


SDL_Texture* wallTexture = nullptr;
TTF_Font* font = nullptr;


std::vector<Enemy> enemies;


Uint32 lastShotTime = 0;
const Uint32 shotCooldown = 300;

// Function prototypes
void handleInput(Player& player, bool& quit, float deltaTime);
void updateEnemies(Player& player, float deltaTime);
void handleShooting(Player& player, SDL_Event& event);
void castRays(SDL_Renderer* renderer, Player& player);
bool isWall(float x, float y);
void renderFloor(SDL_Renderer* renderer);
void renderUI(SDL_Renderer* renderer, Player& player, float fps);
void renderText(SDL_Renderer* renderer, const std::string& message, int x, int y);
void renderCrosshair(SDL_Renderer* renderer);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Initialize SDL_image and SDL_ttf
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        std::cerr << "SDL_image Init Error: " << IMG_GetError() << std::endl;
    if (TTF_Init() == -1)
        std::cerr << "TTF Init Error: " << TTF_GetError() << std::endl;
    
    SDL_Window* window = SDL_CreateWindow("Advanced Raycasting Engine", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // Load wall texture (ensure that "wall.png" is in your project directory)
    wallTexture = IMG_LoadTexture(renderer, "wall.png");
    if (!wallTexture)
        std::cerr << "Failed to load wall texture: " << IMG_GetError() << std::endl;
    
    // Load font for UI (ensure "ARIAL.TTF" is available)
    font = TTF_OpenFont("ARIAL.TTF", 16);
    if (!font)
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    Player player;
    
    // Initialize some enemies at preset positions 
    enemies.push_back(Enemy(10.5f, 10.5f));
    enemies.push_back(Enemy(15.5f, 8.5f));
    enemies.push_back(Enemy(8.5f, 14.5f));
    
    bool quit = false;
    Uint32 lastTime = SDL_GetTicks();
    float fps = 0.0f;
    int frameCount = 0;
    Uint32 fpsTimer = SDL_GetTicks();
    
  
    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // Update FPS counter every second
        frameCount++;
        if (currentTime - fpsTimer >= 1000) {
            fps = frameCount * 1000.0f / (currentTime - fpsTimer);
            fpsTimer = currentTime;
            frameCount = 0;
        }
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
      
            if (event.type == SDL_MOUSEMOTION)
                player.angle += event.motion.xrel * MOUSE_SENSITIVITY;

            if (event.type == SDL_MOUSEBUTTONDOWN)
                handleShooting(player, event);
        }
        
        // Keyboard input for movement, strafing, and rotation
        handleInput(player, quit, deltaTime);
        // Update enemy positions toward the player
        updateEnemies(player, deltaTime);
        
        // Clear screen with a sky-blue color for the ceiling
        SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255); // steel blue sky
        SDL_RenderClear(renderer);
        
        // Draw the floor gradient
        renderFloor(renderer);
        
        // Cast rays and render the 3D view with textured walls (if available)
        castRays(renderer, player);
        
        // Render crosshair in the center
        renderCrosshair(renderer);
        
        // Render the minimap and on-screen UI (FPS, player coordinates, enemies left)
        renderUI(renderer, player, fps);
        

        SDL_RenderPresent(renderer);
    }
    
    // Cleanup resources
    if (wallTexture) SDL_DestroyTexture(wallTexture);
    if (font) TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    return 0;
}

void handleInput(Player& player, bool& quit, float deltaTime) {
    const float moveSpeed = 3.0f * deltaTime;    // movement speed adjusted by deltaTime
    const float rotationSpeed = 2.0f * deltaTime;  // extra rotation via keys
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    
    // Forward/Backward movement
    if (keyState[SDL_SCANCODE_W]) {
        float newX = player.x + cos(player.angle) * moveSpeed;
        float newY = player.y + sin(player.angle) * moveSpeed;
        if (!isWall(newX, player.y))
            player.x = newX;
        if (!isWall(player.x, newY))
            player.y = newY;
    }
    if (keyState[SDL_SCANCODE_S]) {
        float newX = player.x - cos(player.angle) * moveSpeed;
        float newY = player.y - sin(player.angle) * moveSpeed;
        if (!isWall(newX, player.y))
            player.x = newX;
        if (!isWall(player.x, newY))
            player.y = newY;
    }
    
    // Strafing (left/right)
    if (keyState[SDL_SCANCODE_A]) {
        float newX = player.x - cos(player.angle + M_PI/2) * moveSpeed;
        float newY = player.y - sin(player.angle + M_PI/2) * moveSpeed;
        if (!isWall(newX, player.y))
            player.x = newX;
        if (!isWall(player.x, newY))
            player.y = newY;
    }
    if (keyState[SDL_SCANCODE_D]) {
        float newX = player.x + cos(player.angle + M_PI/2) * moveSpeed;
        float newY = player.y + sin(player.angle + M_PI/2) * moveSpeed;
        if (!isWall(newX, player.y))
            player.x = newX;
        if (!isWall(player.x, newY))
            player.y = newY;
    }
    
    // Additional rotation via arrow keys
    if (keyState[SDL_SCANCODE_LEFT])
        player.angle -= rotationSpeed;
    if (keyState[SDL_SCANCODE_RIGHT])
        player.angle += rotationSpeed;
    

    if (keyState[SDL_SCANCODE_ESCAPE])
        quit = true;
}

void updateEnemies(Player& player, float deltaTime) {
    // For each enemy, move them slowly toward the player if they're alive
    for (auto &enemy : enemies) {
        if (!enemy.alive)
            continue;
        float dx = player.x - enemy.x;
        float dy = player.y - enemy.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance > 0.5f) { // stop moving when too close
            enemy.x += (dx / distance) * enemy.speed * deltaTime;
            enemy.y += (dy / distance) * enemy.speed * deltaTime;
        }
    }
}

void handleShooting(Player& player, SDL_Event& event) {
    if (event.button.button != SDL_BUTTON_LEFT)
        return;
    
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShotTime < shotCooldown)
        return; 
    
    lastShotTime = currentTime;

    // We calculate the angle between the player's view and the enemy position.
    for (auto &enemy : enemies) {
        if (!enemy.alive)
            continue;
        float dx = enemy.x - player.x;
        float dy = enemy.y - player.y;
        float angleToEnemy = atan2(dy, dx);
        float angleDiff = fabs(player.angle - angleToEnemy);
        if (angleDiff > M_PI) angleDiff = 2 * M_PI - angleDiff;
        // If enemy is roughly in the center of view and within range
        if (angleDiff < 0.1f && sqrt(dx*dx + dy*dy) < 8.0f) {
            enemy.health -= 50; // reduce enemy health
            if (enemy.health <= 0) {
                enemy.alive = false;
                std::cout << "Enemy eliminated!\n";
            }
        }
    }
}

void castRays(SDL_Renderer* renderer, Player& player) {
    // For each vertical screen column, cast a ray
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        float rayAngle = (player.angle - player.fov / 2.0f) + (float(x) / SCREEN_WIDTH) * player.fov;
        float distanceToWall = 0.0f;
        bool hitWall = false;
        float eyeX = cos(rayAngle);
        float eyeY = sin(rayAngle);
        
        while (!hitWall && distanceToWall < 20.0f) {
            distanceToWall += 0.05f;
            int testX = int(player.x + eyeX * distanceToWall);
            int testY = int(player.y + eyeY * distanceToWall);
            
            // Check map bounds
            if (testX < 0 || testX >= MAP_WIDTH || testY < 0 || testY >= MAP_HEIGHT) {
                hitWall = true;
                distanceToWall = 20.0f;
            } else {
                if (worldMap[testX][testY] == 1) {
                    hitWall = true;
                    
                    float blockMidX = testX + 0.5f;
                    float blockMidY = testY + 0.5f;
                    float hitX = player.x + eyeX * distanceToWall;
                    float hitY = player.y + eyeY * distanceToWall;
                    float angleBetween = atan2(hitY - blockMidY, hitX - blockMidX);
                    float textureX = fabs(cos(angleBetween));
                    
                    // Determine the height of the wall slice
                    int lineHeight = int(SCREEN_HEIGHT / distanceToWall);
                    int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
                    if (drawStart < 0) drawStart = 0;
                    int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
                    if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;
                    
                    SDL_Rect srcRect;
                    srcRect.x = int(textureX * 64); 
                    srcRect.y = 0;
                    srcRect.w = 1;
                    srcRect.h = 64;
                    
                    SDL_Rect destRect;
                    destRect.x = x;
                    destRect.y = drawStart;
                    destRect.w = 1;
                    destRect.h = drawEnd - drawStart;
                    
                    // Use texture if loaded, otherwise fallback to color shading
                    if (wallTexture)
                        SDL_RenderCopy(renderer, wallTexture, &srcRect, &destRect);
                    else {
                        Uint8 color = Uint8(255 / (1 + distanceToWall * distanceToWall * 0.1f));
                        SDL_SetRenderDrawColor(renderer, color, color, color, 255);
                        SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
                    }
                    break; 
                }
            }
        }
    }
}

bool isWall(float x, float y) {
    int ix = int(x);
    int iy = int(y);
    if (ix < 0 || ix >= MAP_WIDTH || iy < 0 || iy >= MAP_HEIGHT)
        return true;
    return worldMap[ix][iy] == 1;
}

void renderFloor(SDL_Renderer* renderer) {
    // Draw a smooth gradient for the floor from the screen's midpoint to the bottom
    for (int y = SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT; y++) {
        float factor = float(y - SCREEN_HEIGHT / 2) / (SCREEN_HEIGHT / 2);
        // Use a gradient from dark gray to a lighter shade
        Uint8 r = Uint8((1 - factor) * 80 + factor * 20);
        Uint8 g = Uint8((1 - factor) * 80 + factor * 20);
        Uint8 b = Uint8((1 - factor) * 80 + factor * 20);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
}

void renderUI(SDL_Renderer* renderer, Player& player, float fps) {
    // Draw a minimap in the top-left corner
    int mapScale = 10;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect tile = { x * mapScale, y * mapScale, mapScale, mapScale };
            if (worldMap[x][y] == 1)
                SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); 
            else
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); 
            SDL_RenderFillRect(renderer, &tile);
        }
    }
    // Draw the player on the minimap
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect playerRect = { int(player.x * mapScale) - 2, int(player.y * mapScale) - 2, 4, 4 };
    SDL_RenderFillRect(renderer, &playerRect);
    
    // Draw the player's view direction as a line
    int lineLength = 10;
    SDL_RenderDrawLine(renderer, 
        int(player.x * mapScale),
        int(player.y * mapScale),
        int(player.x * mapScale + cos(player.angle) * lineLength),
        int(player.y * mapScale + sin(player.angle) * lineLength)
    );
    
    // Draw enemies on the minimap 
    SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // orange for enemies
    for (auto &enemy : enemies) {
        if (!enemy.alive) continue;
        SDL_Rect enemyRect = { int(enemy.x * mapScale) - 2, int(enemy.y * mapScale) - 2, 4, 4 };
        SDL_RenderFillRect(renderer, &enemyRect);
    }
    
    // Render on-screen text (FPS, player position, enemies left) if the font is loaded
    if (font) {
        int aliveEnemies = 0;
        for (auto &enemy : enemies)
            if (enemy.alive) aliveEnemies++;
        std::string info = "FPS: " + std::to_string(int(fps)) +
                           " | Pos: (" + std::to_string(int(player.x)) + "," +
                           std::to_string(int(player.y)) + ")" +
                           " | Enemies: " + std::to_string(aliveEnemies);
        renderText(renderer, info, 10, SCREEN_HEIGHT - 30);
    }
}

void renderText(SDL_Renderer* renderer, const std::string& message, int x, int y) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, message.c_str(), white);
    if (!surface) {
        std::cerr << "TTF RenderText Error: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "CreateTexture Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    SDL_Rect dstRect = { x, y, 0, 0 };
    SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
}

void renderCrosshair(SDL_Renderer* renderer) {
    // Render a simple crosshair in the center of the screen
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, cx - 10, cy, cx + 10, cy);
    SDL_RenderDrawLine(renderer, cx, cy - 10, cx, cy + 10);
}
