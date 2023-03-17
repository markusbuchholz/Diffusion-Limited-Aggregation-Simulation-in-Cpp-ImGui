// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <vector>
#include <math.h>
#include <random>
#include <algorithm>
#include <Eigen/Dense>

//---------------------------------------------------------------

bool SIMULATION = true;
float dt = 6.0f;

int w = 500;
int h = 500;
float radius = 5.0f;

//---------------------------------------------------------------

int generateRandom(int n)
{

	std::random_device engine;
	std::mt19937 generator(engine());
	std::uniform_int_distribution<int> distrib(0, n - 1);
	return distrib(generator);
}
//---------------------------------------------------------------
int randomXY()
{
	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_int_distribution<int> dist(0, 1);

	return dist(engine) == 0 ? -1 : 1;
}
//---------------------------------------------------------------

std::tuple<int, int> initPos()
{

	int x = generateRandom(w);
	int y = generateRandom(h);

	int edge = generateRandom(4);

	if (edge == 0)
	{
		return std::make_tuple(x, 0);
	}
	else if (edge == 1)
	{
		return std::make_tuple(w - 1, y);
	}
	else if (edge == 2)
	{
		return std::make_tuple(x, h - 1);
	}

	return std::make_tuple(0, y);
}

//---------------------------------------------------------------

struct Walker
{

	float x;
	float y;
	int stuck;
	float color;

	Walker()
	{
		auto pos = initPos();
		this->x = (float)std::get<0>(pos);
		this->y = (float)std::get<1>(pos);
		this->stuck = 0;
	}
	void move()
	{
		int dx = randomXY();

		if (this->x + dx * dt <= dt)
		{
			this->x = this->x + dt;
		}

		else if (this->x + dx * dt >= w)
		{
			this->x = this->x - dt;
		}
		else
		{
			this->x = this->x + dx * dt;
		}

		int dy = randomXY();

		if (this->y + dy * dt <= dt)
		{
			this->y = this->y + dt;
		}

		else if (this->y + dy * dt >= h)
		{
			this->y = this->y - dt;
		}
		else
		{
			this->y = this->y + dy * dt;
		}
	}
};

//---------------------------------------------------------------

bool checkIfCollision(Walker a, Walker b)
{

	auto dist = std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));

	return dist < 2 * radius ? true : false;
}

//---------------------------------------------------------------

int main(int argc, char const *argv[])
{

	SIMULATION = true;

	std::string title = "3D Projection";
	initImgui(w, h, title);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(89.0f / 255.0, 88.0f / 255.0, 87.0f / 255.0, 1.00f);
	ImVec4 line_color = ImVec4(255.0f / 255.0, 255.0f / 255.0, 255.0f / 255.0, 1.00f);
	ImVec4 dot_color = ImVec4(255.0f / 255.0, 255.0f / 255.0, 0.0f / 255.0, 1.00f);

	bool flag = true;
	int maxWalkers = 500;
	float cx = w / 2.0;
	float cy = h / 2.0;

	std::vector<Walker> walker_vec;
	std::vector<Walker> walker_stucked;

	for (int ii = 0; ii < maxWalkers; ii++)
	{
		Walker w;
		walker_vec.push_back(w);
	}

	Walker center;
	center.x = cx;
	center.y = cy;
	center.stuck = 1;
	center.color = 0;
	walker_stucked.push_back(center);

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		for (auto &w : walker_vec)
		{
			if (w.stuck == 0)
			{

				w.move();
			}
		}

		for (int ii = 0; ii < walker_vec.size(); ii++)
		{
			if (walker_vec[ii].stuck == 0)
			{

				for (int jj = 0; jj < walker_stucked.size(); jj++)
				{

					if (checkIfCollision(walker_vec[ii], walker_stucked[jj]) == true)
					{

						float dist = sqrt(std::pow(walker_vec[ii].x - walker_stucked[0].x, 2) + std::pow(walker_vec[ii].y - walker_stucked[0].y, 2));
						walker_vec[ii].color = dist / 255.0f;
						walker_vec[ii].stuck = 1;
						walker_stucked.push_back(walker_vec[ii]);
						break;
					}
				}
			}
		}

		for (auto &w : walker_stucked)
		{
			ImVec4 pix_color = ImVec4(0.0f, 0.0f, w.color, 1.0f);

			draw_list->AddCircleFilled({w.x, w.y}, radius, ImColor(pix_color));
		}

		for (auto &w : walker_vec)
		{
			if (w.stuck == 0)
			{

				draw_list->AddCircleFilled({w.x, w.y}, radius, ImColor(line_color));
			}
		}

		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
