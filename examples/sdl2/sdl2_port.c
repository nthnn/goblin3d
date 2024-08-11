#include <SDL2/SDL.h>
#include <goblin3d.h>  // Ensure this is compatible with your platform

// Window and renderer
SDL_Window* window;
SDL_Renderer* renderer;

// Goblin3D object
goblin3d_obj_t cube;

/**
 * Function to draw a line between two points on the SDL2 renderer.
 */
void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

bool initialize() {
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Create SDL window
    window = SDL_CreateWindow("Goblin3D with SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 360, 240, SDL_WINDOW_SHOWN);
    if(!window) {
        SDL_Log("Could not create window: %s", SDL_GetError());
        return false;
    }

    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        SDL_Log("Could not create renderer: %s", SDL_GetError());
        return false;
    }

    // Set the drawing color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    return true;
}

void cleanup() {
    // Destroy renderer and window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if(!initialize())
        return 1;

    // Define the 3D coordinates of the cube's vertices
    float cube_points[9][3] = {
        { -1.0,  -1.0,   1.0 },
        {  1.0,  -1.0,   1.0 },
        {  1.0,   1.0,   1.0 },
        { -1.0,   1.0,   1.0 },
        { -1.0,  -1.0,  -1.0 },
        {  1.0,  -1.0,  -1.0 },
        {  1.0,   1.0,  -1.0 },
        { -1.0,   1.0,  -1.0 },
        {  0.0,   3.0,   0.0 }
    };

    // Define the edges of the cube, connecting pairs of vertices
    uint8_t cube_edges[16][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // Base edges
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Top edges
        {0, 4}, {1, 5}, {2, 6}, {3, 7},  // Vertical edges
        {2, 8}, {3, 8}, {6, 8}, {7, 8}   // Pyramid edges
    };

    // Initialize the Goblin3D object (cube) with 9 points and 16 edges
    if(!goblin3d_init(&cube, 9, 16)) {
        printf("Failed to initialize Goblin3D object.\n");
        cleanup();

        return 1;
    }

    // Set the scaling factor for the 3D object
    cube.scale_size = 120.0;

    // Set the initial rotation angles
    cube.x_angle_deg = 20.0;
    cube.y_angle_deg = 0.0;
    cube.z_angle_deg = 0.0;

    // Set the initial offsets to center the object on the screen
    cube.x_offset = 160;
    cube.y_offset = 120;

    // Copy the predefined cube points to the Goblin3D object's original points array
    for(uint8_t i = 0; i < 9; i++)
        for(uint8_t j = 0; j < 3; j++)
            cube.orig_points[i][j] = cube_points[i][j];

    // Copy the predefined cube edges to the Goblin3D object's edges array
    for(uint8_t i = 0; i < 16; i++)
        for(uint8_t j = 0; j < 2; j++)
            cube.edges[i][j] = cube_edges[i][j];

    // Main loop
    bool quit = false;
    SDL_Event event;

    while(!quit) {
        // Delay for a short period to control frame rate
        SDL_Delay(10);

        // Handle events
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT)
            quit = true;

        // Update the rotation angles for a rotating effect
        cube.x_angle_deg = fmod(cube.x_angle_deg + 1.0, 360.0);
        cube.y_angle_deg = fmod(cube.y_angle_deg + 1.0, 360.0);
        cube.z_angle_deg = fmod(cube.z_angle_deg + 1.0, 360.0);

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Pre-calculate transformations
        goblin3d_precalculate(&cube);

        // Set white as line color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Render the 3D object
        goblin3d_render(&cube, &drawLine);

        // Present the rendered image
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
