#include "desktop.hpp"

#include "../external/glfw.hpp"

namespace dsk
{
       std::vector<Window *> _windows;

       void init()
       {
              if (!glfwInit())
              {
                     throw std::runtime_error("failed to init glfw");
              }
       }
       void end()
       {
              glfwTerminate();
       }
       void refresh()
       {
              glfwPollEvents();
              for (Window *window : _windows)
              {
                     window->m_should_be_destroyed = glfwWindowShouldClose((GLFWwindow *)window->m_handle);
              }
       }
#ifdef LINUX
       Display *get_x11_display()
       {
              return glfwGetX11Display();
       }
#endif //LINUX

       void Window::create(int width, int height, const char *title, bool resizable)
       {
              glfwDefaultWindowHints();
              glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
              glfwWindowHint(GLFW_RESIZABLE, resizable);
              glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
              m_handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
              _windows.push_back(this);
       }
       void Window::destroy()
       {
              glfwDestroyWindow((GLFWwindow *)m_handle);
              for (size_t i = 0; i < _windows.size(); ++i)
              {
                     if (_windows[i] = this)
                     {
                            _windows.erase(_windows.begin() + i);
                            return;
                     }
              }
              throw std::runtime_error("failed to remove the window");
       }
       void Window::show()
       {
              glfwShowWindow((GLFWwindow *)m_handle);
       }
       void Window::hide()
       {
              glfwHideWindow((GLFWwindow *)m_handle);
       }
       bool Window::is_pressed(KeyCode key)
       {
              return glfwGetKey((GLFWwindow *)m_handle, (int)key);
       }
#ifdef LINUX
       ::Window Window::get_x11_window()
       {
              return glfwGetX11Window((GLFWwindow *)m_handle);
       }
#endif //LINUX
}