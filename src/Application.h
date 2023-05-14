#ifndef  _APP_
#define _APP_

class Window;

class Application
{
public:
	Application(char* name, int windowW, int windowH);
	~Application();
public:
	void Run();
private:
	char* m_app_name;
	Window* m_window;
};

#endif

