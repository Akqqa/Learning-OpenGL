#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

// Process input callback per frame
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}


int main()
{

    // Initialise version 3.3, with core profile
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a window 800 by 600 named LearnOpenGL
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

    // Fail
    if (window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Idk - make it the current window ig
    glfwMakeContextCurrent(window);

    // load the GLAD stuff and fail if not
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // Usnure how viewport is diff to window. i think,,, viewport is rendering, window is hysical window
    glViewport(0, 0, 800, 600);

    // Callback to change viewport size to match window when changes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // CREATING A TRIANGLE ---------------------------------------------------------------------------------------------------------------

    // VERTEX SHADER - takes vertices and normalsies the 3d coordinates
    // Premade shader - simplest possible. Simply take the input and send to output gl_Position:     (Normally, we have to normalise first)
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the shader and stores id in vertexSgader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //Takes the shader object to compile, how many strigns as source code, the actual sorce code, and NULL
    glCompileShader(vertexShader);
    // Check compilation success:
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // checks if compilation successful. if failed, retrieve error and print
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // FRAGMENT SHADER - calculates final colour of a pixel (in this case, ORANGE)
    // Same steps as before
    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "} \n";
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check success again
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // SHADER PROGRAM - links shaders to be activated when rendering objects
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check success
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::SHADERPROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glUseProgram(shaderProgram); // Every rendering call will now use this program object
    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    // Create VAO - which saves a configuration to draw when used
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); // unsure exactly how this works in relation to vbo?

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };  
    unsigned int VBO;
    glGenBuffers(1, &VBO); // Generates 1 buffer and writes the id of it to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binds the buffer id VBO to a GL_ARRAY_BUFFER target - which is a vertex buffer object
    // GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    // GL_STATIC_DRAW: the data is set only once and used many times.
    // GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    // Triangle vertices dont change, used a lot, stays same for every render call son we use static draw
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copies vertex data into the buffer - currently bound to VBO

    // Element buffer for specifiying which vertices to draw (cuts down on shared points for data storage)
    unsigned int indices[] {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    

    // Tell opengl how to interperet the vertex data:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Vertex to configure (we specified location = 0 in the shader), size (vec3 so 3 values), data type, dont normalise, stride (space between consecutive verctices), offset data begins in buffer.
    glad_glEnableVertexAttribArray(0); // Location as argument

    glBindVertexArray(0); // Unbind VAO

    // ---------------------------------------------------------------------------------------------------------------------------------------

    // Continually swaps buffers and polls for events
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Rendering steps
        // Sets the clear colour, then clears it with that colour
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Render triangle
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
  
    glfwTerminate();
    return 0;
}
