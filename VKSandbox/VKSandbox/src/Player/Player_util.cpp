#include "Player.h"
#include "Config/Config.h"
#include "Viewport/ViewportManager.h"

glm::ivec2 Player::GetViewportCenter() {
    const Resolutions& resolutions = Config::GetResolutions();
    const Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    if (!viewport->IsVisible()) return glm::ivec2(0, 0);

    int width = resolutions.ui.x * viewport->GetSize().x;
    int height = resolutions.ui.y * viewport->GetSize().y;
    int xLeft = resolutions.ui.x * viewport->GetPosition().x;
    int xRight = xLeft + width;
    int yTop = resolutions.ui.y * (1.0f - viewport->GetPosition().y - viewport->GetSize().y);
    int yBottom = yTop + height;
    int centerX = xLeft + (width / 2);
    int centerY = yTop + (height / 2);
    return glm::ivec2(centerX, centerY);
}