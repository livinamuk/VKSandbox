
// Computes the player index (0-3) using only integer math, avoiding branching enitrely
int GetPlayerIndex(vec2 fragCoord, vec2 renderTargetSize, int splitScreenMode) {
    vec2 uv = fragCoord / renderTargetSize;
    int isTwoPlayer = (1 - (splitScreenMode - 1) * (splitScreenMode - 1)); 
    int isFourPlayer = max((1 - (splitScreenMode - 2) * (splitScreenMode - 2)), 0);
    int row = (1 - int(uv.y * 2)) * (isTwoPlayer + isFourPlayer);
    int col = int(uv.x * 2) % 2 * isFourPlayer;
    return col + (row * 2 * isFourPlayer) + (row * isTwoPlayer);
}