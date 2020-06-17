#include "halley/core/graphics/render_target/render_target_texture.h"
#include <gsl/gsl_assert>
#include "graphics/texture.h"

using namespace Halley;

void TextureRenderTarget::setTarget(int attachmentNumber, std::shared_ptr<Texture> tex)
{
	Expects(attachmentNumber >= 0);
	Expects(attachmentNumber < 8);
	Expects(tex != std::shared_ptr<Texture>());

	if (int(attachments.size()) <= attachmentNumber) {
		attachments.resize(attachmentNumber + 1);
	}
	attachments[attachmentNumber] = tex;

	dirty = true;
}

std::shared_ptr<Texture> TextureRenderTarget::getTexture(int attachmentNumber) const
{
	return attachments.at(attachmentNumber);
}

void TextureRenderTarget::setDepthTexture(std::shared_ptr<Texture> tex)
{
	depth = tex;
	dirty = true;
}

std::shared_ptr<Texture> TextureRenderTarget::getDepthTexture() const
{
	return depth;
}

Rect4i TextureRenderTarget::getViewPort() const
{
	return viewPort ? viewPort.get() : Rect4i(Vector2i(0, 0), getTexture(0)->getSize());
}

void TextureRenderTarget::setViewPort(Rect4i vp)
{
	viewPort = vp;
}

void TextureRenderTarget::resetViewPort()
{
	viewPort = Maybe<Rect4i>();
}
