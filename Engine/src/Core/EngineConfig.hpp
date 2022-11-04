#pragma once

enum RenderingAPI {
	OpenGL_Api
};

// Could get loaded from some Graphics.ini etc. beforehand
const static RenderingAPI RendereringAPI = RenderingAPI::OpenGL_Api;

const static int AntiAliasing = 2;