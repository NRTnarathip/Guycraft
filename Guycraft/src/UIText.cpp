#include "UIText.h"
#include "ResourceManager.h"
#include "TextRenderer.h"
#include "GUI.h"

UIText::UIText() {
}
void UIText::render() {
	auto tr = TextRenderer::GetInstance();
	auto rect = m_uiObject->rect;
	auto pos = rect.getPosition();
	int totalWidth = 0;
	int maxFontHeight = 0;
	// Calculate total width
	auto fontScale = (fontSize / 48.f) * rect.scale;

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		const auto& charInfo = tr->Characters[*c];
		totalWidth += charInfo.Advance * fontScale.x;
		if (charInfo.Size.y * fontScale.y > maxFontHeight) {
			maxFontHeight = charInfo.Size.y * fontScale.y;
		}
	}
	//printf("totalwidth %d\n", totalWidth);
	if (alignment.lineVertical == Alignment::Center) {
		pos.x -= totalWidth / 2.f;
	}
	else if (alignment.lineVertical == Alignment::Right) {
		pos.x -= totalWidth;
	}
	if (alignment.lineHorizontal == Alignment::Center) {
		pos.y -= maxFontHeight / 2.f;
	}
	else if (alignment.lineHorizontal == Alignment::Right) {
		pos.y -= maxFontHeight;
	}

	tr->renderText(text, pos.x, pos.y, fontScale, fontColor);
}