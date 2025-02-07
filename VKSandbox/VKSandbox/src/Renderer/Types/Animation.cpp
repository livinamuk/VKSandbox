#include "Animation.h"
#include "Util/Util.h"

float Animation::GetTicksPerSecond() {
    return m_ticksPerSecond != 0 ? m_ticksPerSecond : 25.0f;;
}

void Animation::SetFileInfo(FileInfo fileInfo) {
    m_fileInfo = fileInfo;
}

FileInfo Animation::GetFileInfo() {
    return m_fileInfo;
}

LoadingState& Animation::GetLoadingState() {
    return m_loadingState;
}

void Animation::SetLoadingState(LoadingState loadingState) {
    m_loadingState = loadingState;
}

const std::string& Animation::GetName() {
    return m_fileInfo.name;
}
