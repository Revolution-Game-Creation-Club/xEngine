#include "xEngine.h"
#include "xRenderWindowImpl.h"

extern "C" {
	extern void glXSwapBuffers( Display *dpy, XID drawable );
}

xRenderWindow::xRenderWindow(xUInt32 width, xUInt32 height) : mWidth(width), mHeight(height){
	pImpl = new Impl;
	pImpl->mClosed = false;
	pImpl->mpDisplay = XOpenDisplay(NULL);
	pImpl->mWindow = XCreateSimpleWindow(pImpl->mpDisplay, DefaultRootWindow(pImpl->mpDisplay),
			0, 0, width, height, CopyFromParent, CopyFromParent, CopyFromParent);
	XSelectInput(pImpl->mpDisplay, pImpl->mWindow, ExposureMask);

	pImpl->mWmDeleteWindow = XInternAtom(pImpl->mpDisplay, "WM_DELETE_WINDOW", 1);
	XSetWMProtocols(pImpl->mpDisplay, pImpl->mWindow, &pImpl->mWmDeleteWindow, 1);
}

xRenderWindow::~xRenderWindow(){
	XDestroyWindow(pImpl->mpDisplay, pImpl->mWindow);
	XCloseDisplay(pImpl->mpDisplay);
	xSAFE_DELETE(pImpl);
}

void xRenderWindow::ProcessMessages(){
	XEvent event;
	do{
		XNextEvent(pImpl->mpDisplay, &event);
		if ((event.type == ClientMessage) && (event.xclient.data.l[0] == pImpl->mWmDeleteWindow))
			pImpl->mClosed = true;
	}while(XPending(pImpl->mpDisplay));
}

void xRenderWindow::Show(){
	XMapWindow(pImpl->mpDisplay, pImpl->mWindow);
	XFlush(pImpl->mpDisplay);
}

void xRenderWindow::Hide(){
	XUnmapWindow(pImpl->mpDisplay, pImpl->mWindow);
}

bool xRenderWindow::IsClosed() const {
	return pImpl->mClosed;
}

xUInt32 xRenderWindow::Handle() const {
	return (xUInt32)pImpl->mWindow;
}

void xRenderWindow::Present(){
	glXSwapBuffers(pImpl->mpDisplay, pImpl->mWindow);
}
