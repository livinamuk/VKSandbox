#include "UIBackEnd.h"
#include "Mesh2D.h"
#include "TextBlitter.h"
#include "../AssetManagement/AssetManager.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Debug.h"
#include "../Config/Config.h"

namespace UIBackEnd {

    Mesh2D g_uiMesh;
    std::vector<Vertex2D> g_vertices;
    std::vector<uint32_t> g_indices;
    std::vector<UIRenderItem> g_renderItems;

    void Init() {
        // Export standard font (no need to do every init but YOLO ¯\_(ツ)_/¯
        std::string name = "StandardFont";
        std::string characters = R"(!"#$%&'*+,-./0123456789:;<=>?_ABCDEFGHIJKLMNOPQRSTUVWXYZ\^_`abcdefghijklmnopqrstuvwxyz )";
        std::string textureSourcePath = "res/fonts/raw_images/standard_font/";
        std::string outputPath = "res/fonts/";
        FontSpriteSheetPacker::Export(name, characters, textureSourcePath, outputPath);

        name = "AmmoFont";
        characters = "0123456789/";
        textureSourcePath = "res/fonts/raw_images/ammo_font/";
        FontSpriteSheetPacker::Export(name, characters, textureSourcePath, outputPath);

        // Import fonts
        FontSpriteSheet standardFont = FontSpriteSheetPacker::Import("res/fonts/StandardFont.json");
        FontSpriteSheet ammoFont = FontSpriteSheetPacker::Import("res/fonts/AmmoFont.json");
        TextBlitter::AddFont(standardFont);
        TextBlitter::AddFont(ammoFont);
    }

    void Update() {
        
        if (Debug::IsDebugTextVisible()) {
            BlitText(Debug::GetText(), "StandardFont", 0, 0, Alignment::TOP_LEFT, 2.0f);
        }

        const Resolutions& resolutions = Config::GetResolutions();
     //   BlitText("0123456789/", "AmmoFont", 100, 250, 2.0f);

        //if (AssetManager::LoadingComplete()) {
        //    BlitTexture("ui_test", glm::ivec2(250, 150), Alignment::TOP_LEFT, glm::vec4(1.0, 0.0, 0.0, 0.5), glm::ivec2(-1, -1));
        //    BlitText("Might leave in a body bag,", "StandardFont", 250, 150, 2.0f);
        //    BlitText("Never in cuffs.", "StandardFont", 250, 180, 2.0f);
        //}       

        if (BackEnd::GetAPI() == API::OPENGL) {
            g_uiMesh.GetGLMesh2D().UpdateVertexBuffer(g_vertices, g_indices);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TO DO   
            // g_mesh.GetVKMesh2D().UpdateVertexBuffer(g_vertices, g_indices);
        }
        g_vertices.clear();
        g_indices.clear();
    }

    void EndFrame() {
        g_renderItems.clear();
    }

    void BlitText(const std::string& text, const std::string& fontName, int originX, int originY, Alignment alignment, float scale, TextureFilter textureFilter) {
        FontSpriteSheet* fontSpriteSheet = TextBlitter::GetFontSpriteSheet(fontName);
        if (!fontSpriteSheet) {
            std::cout << "UIBackEnd::BlitText() failed to find " << fontName << "\n";
            return;
        }
        int baseVertex = g_vertices.size();

        const Resolutions& resolutions = Config::GetResolutions();

        MeshData2D meshData = TextBlitter::BlitText(text, fontName, originX, originY, resolutions.ui, alignment, scale, baseVertex);
        g_vertices.insert(std::end(g_vertices), std::begin(meshData.vertices), std::end(meshData.vertices));
        g_indices.insert(std::end(g_indices), std::begin(meshData.indices), std::end(meshData.indices));

        UIRenderItem& renderItem = g_renderItems.emplace_back();
        renderItem.baseVertex = 0;
        renderItem.baseIndex = g_indices.size() - meshData.indices.size();
        renderItem.indexCount = meshData.indices.size();
        renderItem.textureIndex = AssetManager::GetTextureIndexByName(fontName); 
        renderItem.filter = (textureFilter == TextureFilter::NEAREST) ? 1 : 0;
    }

    void BlitTexture(const std::string& textureName, glm::ivec2 location, Alignment alignment, glm::vec4 colorTint, glm::ivec2 size, TextureFilter textureFilter) {
        // Bail if texture not found
        int textureIndex = AssetManager::GetTextureIndexByName(textureName);
        if (textureIndex == -1) {
            std::cout << "BlitTexture() failed. Could not find texture '" << textureName << "'\n";
            return;
        }
        // Get texture dimensions
        Texture* texture = AssetManager::GetTextureByIndex(textureIndex);
        float texWidth = (size.x != -1) ? size.x : texture->GetWidth(0);
        float texHeight = (size.y != -1) ? size.y : texture->GetHeight(0);

        // Alignment
        switch (alignment) {
        case Alignment::CENTERED:
            location.x -= texWidth * 0.5f;
            location.y -= texHeight * 0.5f;
            break;
        case Alignment::TOP_LEFT:
            // No change needed
            break;
        case Alignment::TOP_RIGHT:
            location.x -= texWidth;
            break;
        case Alignment::BOTTOM_LEFT:
            location.y -= texHeight;
            break;
        case Alignment::BOTTOM_RIGHT:
            location.x -= texWidth;
            location.y -= texHeight;
            break;
        case Alignment::CENTERED_HORIZONTAL:
            location.x -= texWidth * 0.5f;
            break;
        case Alignment::CENTERED_VERTICAL:
            location.y -= texHeight * 0.5f;
            break;
        }
        // Calculate normalized device coordinates for quad
        const Resolutions& resolutions = Config::GetResolutions();
        float x0 = (location.x / static_cast<float>(resolutions.ui.x)) * 2.0f - 1.0f;
        float y0 = 1.0f - (location.y / static_cast<float>(resolutions.ui.y)) * 2.0f;
        float x1 = ((location.x + texWidth) / static_cast<float>(resolutions.ui.x)) * 2.0f - 1.0f;
        float y1 = 1.0f - ((location.y + texHeight) / static_cast<float>(resolutions.ui.y)) * 2.0f;

        int baseVertex = g_vertices.size();
        int baseIndex = g_indices.size();

        g_vertices.reserve(baseVertex + 4);
        g_vertices.push_back({ {x0, y0}, {0.0f, 0.0f}, colorTint, textureIndex }); // Bottom-left
        g_vertices.push_back({ {x1, y0}, {1.0f, 0.0f}, colorTint, textureIndex }); // Bottom-right
        g_vertices.push_back({ {x1, y1}, {1.0f, 1.0f}, colorTint, textureIndex }); // Top-right
        g_vertices.push_back({ {x0, y1}, {0.0f, 1.0f}, colorTint, textureIndex }); // Top-left

        g_indices.reserve(baseIndex + 6);
        g_indices.push_back(baseVertex + 0);
        g_indices.push_back(baseVertex + 1);
        g_indices.push_back(baseVertex + 2);
        g_indices.push_back(baseVertex + 0);
        g_indices.push_back(baseVertex + 2);
        g_indices.push_back(baseVertex + 3);

        UIRenderItem& renderItem = g_renderItems.emplace_back();
        renderItem.baseVertex = 0;
        renderItem.baseIndex = baseIndex;
        renderItem.indexCount = 6;
        renderItem.textureIndex = textureIndex;
        renderItem.filter = (textureFilter == TextureFilter::NEAREST) ? 1 : 0;
    }



    Mesh2D& GetUIMesh() {
        return g_uiMesh;
    }

    std::vector<UIRenderItem>& GetRenderItems() {
        return g_renderItems;
    }
}