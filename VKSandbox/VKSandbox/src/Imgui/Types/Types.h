#include "HellTypes.h"
#include "HellFunctionTypes.h"
#include <functional>

struct EditorWindow;
struct EditorButton;

struct FileMenuNode {
    FileMenuNode() = default;
    FileMenuNode(const std::string& text, std::function<void()> callback = nullptr, bool addPadding = true);
    FileMenuNode& AddChild(const std::string& text, std::function<void()> callback = nullptr, bool addPadding = true);
    void CreateImguiElement();

    private:
    std::string m_text;
    std::function<void()> m_callback = nullptr;
    std::vector<FileMenuNode> m_children;
};

struct FileMenu {
    FileMenuNode& AddMenuNode(const std::string& text, std::function<void()> callback = nullptr, bool addPadding = false);
    void CreateImguiElements();

    private:
    std::vector<FileMenuNode> m_menuNodes;
};


struct NewFileWindow {
    NewFileWindow() = default;
    NewFileWindow(const std::string& title, NewFileCallback callback);
    void Show();
    void Close();
    void CreateImGuiElements();
    bool IsVisible();

private:
    std::string m_title;
    bool m_isVisible = false;
    bool m_openedThisFrame = false;
    char m_textBuffer[256] = "";
    NewFileCallback m_callback = nullptr;
};

struct OpenFileWindow {
    OpenFileWindow() = default;
    OpenFileWindow(const std::string& title, const std::string& path, OpenFileCallback callback);
    void Show();
    void Close();
    void CreateImGuiElements();
    bool IsVisible();

    private:
    std::string m_title;
    std::string m_path;
    int m_selectedIndex = -1;
    bool m_isVisible = false;
    bool m_openedThisFrame = false;
    OpenFileCallback m_callback = nullptr;
};

struct EditorButton {
    public:
    EditorButton() = default;
    EditorButton(const std::string& text, void (*callback)(), EditorWindow* parentWindow);
    void CreateImguiElement();
    void CreateCenteredImguiElement(int centerXPos);
    glm::ivec2 CalculateSize();

    private:
    std::string m_text = "";
    EditorWindow* m_parentWindow = nullptr;
    void (*m_callback)() = nullptr;
};

struct EditorWindow {
    public:
    void SetTitle(const std::string& title);
    void AddButton(std::string text, void (*callback)());
    void CreateImGuiElements();
    void Show();
    void Close();
    void ToggleOpenState();
    bool IsVisible();

    private:
    bool m_visible = false;
    std::string m_title = "";
    std::vector<EditorButton> m_buttons;
};