#include "Widget.h"

using namespace drz;

Widget::Widget(int screenX, int screenY, int width, int height) {
  this->screenX = screenX;
  this->screenY = screenY;
  this->width = width;
  this->height = height;
  this->gfx = DrzGraphics::Get();
}

void Widget::SetPosition(int x, int y) {
  this->screenX = x;
  this->screenY = y;
}

void Widget::SetSize(int width, int height) {
  this->width = width;
  this->height = height;
}

//Draw pixel relative to the widget position
void Widget::DrawPixel(int px, int py, Color p) {
  if(gfx == nullptr) return;
  int x = px + screenX;
  int y = py + screenY; 
  // Check if the pixel is out of the widget bounds
  if(x < 0 || x >= width) return;
  if(y < 0 || y >= height) return;
  // Check if the pixel is out of the screen bounds
  if(x >= gfx->GetScreenWidth()) return;
  if(y >= gfx->GetScreenHeight()) return;

  gfx->DrawPixel(x, y, p);
}

//Draw text relative to the widget position
void Widget::DrawText(std::string text, int px, int py, Color c) {
  if(gfx == nullptr) return;
  int x = px + screenX;
  int y = py + screenY; 
  gfx->DrawText(text, x, y, c);
}

//Draw line relative to the widget position
void Widget::DrawLine(int x1, int y1, int x2, int y2, Color c) {
  if(gfx == nullptr) return;
  int x = x1 + screenX;
  int y = y1 + screenY;
  int x_ = x2 + screenX;
  int y_ = y2 + screenY;
  gfx->DrawLine(x, y, x_, y_, c);
}

//Draw rectangle relative to the widget position
void Widget::FillRect(int x, int y, int w, int h, Color c) {
  if(gfx == nullptr) return;
  int x_ = x + screenX;
  int y_ = y + screenY;
  gfx->FillRect(x_, y_, w, h, c);
}

//Clear the widget
void Widget::Clear(Color c) {
  if(gfx == nullptr) return;
  gfx->FillRect(screenX, screenY, width, height, c);
}

void Widget::Clear() {
  Clear(BLACK);
}
