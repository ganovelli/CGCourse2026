#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../common/debugging.h"

std::vector<GLuint> create_grid_indices(int xsize, int ysize){
    std::vector<GLuint> indices;
    int cols = xsize + 1;
    
    for(int i = 0; i < ysize; ++i){
        for(int j = 0; j < xsize; ++j){
            int topLeft = i * cols + j;
            int topRight = i * cols + j + 1;
            int bottomLeft = (i + 1) * cols + j;
            int bottomRight = (i + 1) * cols + j + 1;
            
            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    return indices;
}


GLuint create_box2d(int xsize, int ysize){
    int vertexes = (xsize + 1) * (ysize + 1);
    std::vector<float> positions;
    float xstep {2.f / xsize};
    float ystep {2.f / ysize};

    for(int i{0}; i < ysize + 1; ++i){
        for(int j{0}; j < xsize + 1; ++j){
            int idx = i * (xsize + 1) + j;

            positions.push_back(-1.f + (j * xstep));
            positions.push_back(-1.f + (i * ystep));

            positions.push_back(1.0f);
            positions.push_back(1.0f);
            positions.push_back(1.0f);
        }
    }

    
    GLuint idmat {}; // Reference to points buffer
    glGenBuffers(1, &idmat); 
    glBindBuffer(GL_ARRAY_BUFFER, idmat);

    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    return idmat;
}

int main(int argc, char** argv) {

	GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    // Request OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Ask specifically for the core profile (recommended)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // macOS requires this for 3.2+ contexts
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(512, 512, "code_02_my_first_triangle", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window); 


    // Load GL symbols *after* the context is current
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }


    /* query for the hardware and software specs and print the result on the console*/
    printout_opengl_glsl_info();

    ///* create render data in RAM */
    GLuint positionAttribIndex = 0;
    float positions_colors[] = { 
                        0.0f, 0.0f,     1.0f, 0.0f, 0.0f, // 1st vertex
                        0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 2nd vertex
                        0.5f, 0.5f,     0.0f, 0.0f, 1.0f,// 3rd vertex  
                        0.0f, 0.5f,     1.0f, 1.0f, 1.0f, // 4th vertex
    };


    ///* create  a vertex array object */
    GLuint va;
    glGenVertexArrays(1, &va);
    glBindVertexArray(va);

    ///* create a buffer for the render data in video RAM */
    GLuint positionsBuffer;
    glGenBuffers(1, &positionsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);

    ///* declare what data in RAM are filling the bufferin video RAM */
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions_colors), positions_colors, GL_STATIC_DRAW);

    glEnableVertexAttribArray(positionAttribIndex);
    ///* specify the data format */
    glVertexAttribPointer(positionAttribIndex, 2, GL_FLOAT, false, 5 * sizeof(float), 0);
    ///* declare what data in RAM are filling the bufferin video RAM */

    GLuint colorAttribIndex = 1;
    glEnableVertexAttribArray(colorAttribIndex);
    ///* specify the data format */
    glVertexAttribPointer(colorAttribIndex, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(2 * sizeof(float)));


    GLuint matrix = 2;
    GLuint matrixColor = 3;

    GLuint matid {create_box2d(2,3)};
    glBindBuffer(GL_ARRAY_BUFFER, matid);

    std::vector<GLuint> indices {create_grid_indices(2,3)};

    glEnableVertexAttribArray(matrix);
    glVertexAttribPointer(matrix, 2, GL_FLOAT, false, 5 * sizeof(float), 0);

    glEnableVertexAttribArray(matrixColor);
    glVertexAttribPointer(matrixColor, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    
    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
    


    ///* create a vertex shader */
    std::string  vertex_shader_src = "#version 410\n \
        in vec2 aPosition;\
        in vec3 aColor;\
        out vec3 vColor;\
        uniform float uDelta;\
        void main(void)\
        {\
         gl_Position = vec4(aPosition+vec2(uDelta,0.0), 0.0, 1.0);\
         vColor = aColor;\
        }\
       ";
    const GLchar* vs_source = (const GLchar*)vertex_shader_src.c_str();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vs_source, NULL);
    glCompileShader(vertex_shader);
  

    ///* create a fragment shader */
    std::string   fragment_shader_src = "#version 410 \n \
        layout (location = 0) out vec4 color;\
        in vec3 vColor;\
        uniform float uDelta;\
        void main(void)\
        {\
            color = vec4(vColor+vec3(uDelta,0.0,0.0), 1.0);\
        }";
    const GLchar* fs_source = (const GLchar*)fragment_shader_src.c_str();
    
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fs_source, NULL);
    glCompileShader(fragment_shader);
    

    GLuint program_shader = glCreateProgram();
    glAttachShader(program_shader, vertex_shader);
    glAttachShader(program_shader, fragment_shader);
     
    glBindAttribLocation(program_shader, positionAttribIndex, "aPosition");
    glBindAttribLocation(program_shader, colorAttribIndex, "aColor");
   
    glBindAttribLocation(program_shader, matrix, "aPosition");
    glBindAttribLocation(program_shader, matrixColor, "aColor");

    glLinkProgram(program_shader);

    GLint linked;
    validate_shader_program(program_shader);
    glGetProgramiv(program_shader, GL_LINK_STATUS, &linked);
    if (linked) {
        glUseProgram(program_shader);
    }
    
    GLint loc = glGetUniformLocation(program_shader, "uDelta");

    /* cal glGetError and print out the result in a more verbose style
    * __LINE__ and __FILE__ are precompiler directive that replace the value with the
    * line and file of this call, so you know where the error happened
    */
    check_gl_errors(__LINE__, __FILE__);

    float d = 0.01;
    float delta = 0;

    glClearColor(0.2, 0.2, 0.2, 1);
    while (!glfwWindowShouldClose(window))
    {
        /*
        if (delta < 0 || delta > 0.5)
            d = -d;
        delta += d;
        */

        glUniform1f(loc, delta);

        /* Render here */
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

	return 0;
}