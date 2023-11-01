#include <stdio.h>
#include <math.h>

#include <eh/external/glad.h>
#include <eh/ehMath/ehMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <eh/shader.h>
#include <eh/ehMath/vec3.h>
#include <eh/procGen.h>
#include <eh/transformations.h>
#include <eh/camera.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Square aspect ratio for now. We will account for this with projection later.
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

float prevTime;

const int NUM_CUBES = 4;
eh::Transform cubeTransforms[NUM_CUBES]; 

struct CameraControls
{
	double prevMouseX = 0.0f;
	double prevMouseY = 0.0f; //Mouse position from previous frame
	float yaw = 0.0f;
	float pitch = 0.0f; //Degrees
	float mouseSensitivity = 0.1f; //How fast to turn with mouse
	bool firstMouse = true; //Flag to store initial mouse position
	float moveSpeed = 5.0f; //How fast to move with arrow keys (M/S)

	void moveCamera(GLFWwindow* window, eh::Camera* camera, float deltaTime);
};

void resetCamera(eh::Camera& camera, CameraControls& cameraControls);

int main()
{
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	eh::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	eh::Mesh cubeMesh(eh::createCube(0.5f));

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	eh::Camera camera;
	CameraControls cameraControls;

	resetCamera(camera, cameraControls);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//Calculate deltaTime
		float time = (float)glfwGetTime(); //Timestamp of current frame
		float deltaTime = time - prevTime;
		prevTime = time;

		//Pass deltaTime into moveCamera. Update this function to include a 4th parameter.
		cameraControls.moveCamera(window, &camera, deltaTime);

		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set uniforms
		shader.use(); 

		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::Text("Camera");

			if (ImGui::Button("Reset")) {
				resetCamera(camera, cameraControls);
			}

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");


}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void CameraControls::moveCamera(GLFWwindow* window, eh::Camera* camera, float deltaTime)
{
	bool prevFirstMouse = firstMouse;
	//If right mouse is not held, release cursor and return early.
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		//Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
		return;
	}

	//GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	{
		//Get screen mouse position this frame
		double mouseX, mouseY;

		glfwGetCursorPos(window, &mouseX, &mouseY);

		//If we just started right clicking, set prevMouse values to current position.
		//This prevents a bug where the camera moves as soon as we click.
		if (firstMouse)
		{
			firstMouse = false;
			prevMouseX = mouseX;
			prevMouseY = mouseY;
		}

		float mouseDeltaX = (float)(mouseX - prevMouseX);
		float mouseDeltaY = (float)(mouseY - prevMouseY);

		prevMouseX = mouseX;
		prevMouseY = mouseY;

		yaw += mouseDeltaX * mouseSensitivity;
		pitch -= mouseDeltaY * mouseSensitivity;
		pitch = eh::Clamp(pitch, -89.0f, 890.f);
	}

	{
		float yawRad = eh::Radians(yaw);
		float pitchRad = eh::Radians(pitch);

		eh::Vec3 forward;
		forward.x = cosf(pitchRad) * sinf(yawRad);
		forward.y = sinf(pitchRad);
		forward.z = cosf(pitchRad) * -cosf(yawRad);
		forward = eh::Normalize(forward);

		eh::Vec3 right = eh::Normalize(eh::Cross(forward, eh::Vec3(0, 1, 0)));
		eh::Vec3 up = eh::Normalize(eh::Cross(right, forward));

		float speed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

		float moveDelta = speed * deltaTime;

		if (glfwGetKey(window, GLFW_KEY_W)) {
			camera->position += forward * moveDelta;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			camera->position -= forward * moveDelta;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			camera->position += right * moveDelta;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			camera->position -= right * moveDelta;
		}
		if (glfwGetKey(window, GLFW_KEY_E)) {
			camera->position += up * moveDelta;
		}
		if (glfwGetKey(window, GLFW_KEY_Q)) {
			camera->position -= up * moveDelta;
		}

		//Camera will now look at a position along this forward axis
		camera->target = camera->position + forward;

	}
}

void resetCamera(eh::Camera& camera, CameraControls& cameraControls)
{
	camera.position = eh::Vec3(0, 0, 3);
	camera.target = eh::Vec3(0);
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraControls.yaw = 0.0f;
	cameraControls.pitch = 0.0f;
}
