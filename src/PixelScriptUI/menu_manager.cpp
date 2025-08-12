// menu_manager.cpp
#include "PixelScriptUI/menu_manager.h"
#include <string.h> // For memset, a standard C library function

// Private constructor for the MenuManager singleton.
// It initializes all member variables to a known, safe state.
MenuManager::MenuManager() : mCurrent(nullptr), mDepth(0), mRootCount(0) {
    // Initialize the navigation stack to all null pointers.
    // This is important to prevent accessing uninitialized memory.
    memset(mStack, 0, sizeof(mStack));
    
    // Initialize the root menu list to all null pointers.
    memset(mRootList, 0, sizeof(mRootList));
}

// Pushes a MenuLevel onto the navigation stack.
// This is called when the user navigates into a sub-menu.
void MenuManager::pushStack(MenuLevel* n) {
    if (mDepth < MAX_MENU_LEVEL) {
        mStack[mDepth++] = n;
    }
}

// Pops a MenuLevel from the navigation stack.
// This is called when the user navigates back.
// Returns a pointer to the previous menu level, or nullptr if the stack is empty.
MenuLevel* MenuManager::popStack() {
    if (mDepth > 0) {
        return mStack[--mDepth];
    }
    return nullptr;
}