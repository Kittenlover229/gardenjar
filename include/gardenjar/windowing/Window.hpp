#ifndef GARDENJAR_H__WINDOWING__WINDOW
#define GARDENJAR_H__WINDOWING__WINDOW

#include <memory>
#include <string>

namespace gardenjar::windowing {

struct WindowManager;

struct Window {
  Window(std::u8string);
  virtual ~Window();

  virtual void draw(WindowManager&);
  virtual bool keep_alive();

  void close();

 protected:
  virtual void draw_impl(WindowManager&);

 private:
  bool _keep_alive = true;
  std::u8string window_name;
};

}  // namespace gardenjar::windowing

#endif
