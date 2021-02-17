#ifndef  _APP_
#define _APP_

class Window;

class Application
{
public:
	Application(char* name, int windowCount, int windowW, int windowH);
	virtual ~Application();
public:
	void Run();
protected:
private:
	bool m_running;
	char* m_app_name;
	int m_windows_count;
	Window** m_windows = nullptr;
};

#endif // ! _APP_
