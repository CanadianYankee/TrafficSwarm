// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include <unordered_set>
#include <vector>
#include <fstream>
#include <memory>
#include <algorithm>
#include <afxcontrolbars.h>

#define WM_USER_CHILD_CLOSING WM_USER+10
#define WM_USER_TRIAL_ENDED WM_USER+11
#define WM_USER_RESULTS_SELECTED WM_USER+12

#endif //PCH_H
