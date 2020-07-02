#pragma once
#include "../scene_editor_gizmo.h"

namespace Halley {
	class UIList;

	enum class PolygonGizmoMode {
		Move,
		Append,
		Insert,
		Delete
	};

	template <>
	struct EnumNames<PolygonGizmoMode> {
		constexpr std::array<const char*, 4> operator()() const {
			return{{
					"move",
					"append",
					"insert",
					"delete"
				}};
		}
	};

	class PolygonGizmo final : public SceneEditorGizmo {
	public:
		PolygonGizmo(String componentName, String fieldName, const ConfigNode& options, UIFactory& factory);
		void update(Time time, const SceneEditorInputState& inputState) override;
		void draw(Painter& painter) const override;
		std::shared_ptr<UIWidget> makeUI() override;

	protected:
		void onEntityChanged() override;

	private:
		UIFactory& factory;
		String componentName;
		String fieldName;
		bool isOpenPolygon;
		Colour4f colour;

		VertexList lastStored;
		VertexList vertices;
		std::optional<Vertex> preview;
		size_t previewIndex = 0;
		std::vector<SceneEditorGizmoHandle> handles;

		PolygonGizmoMode mode = PolygonGizmoMode::Move;
		std::shared_ptr<UIList> uiList;

		VertexList readPoints();
		void writePoints(const VertexList& ps);
		void writePointsIfNeeded();

		void loadHandlesFromVertices();
		Rect4f getHandleRect(Vector2f pos, float size) const;
		SceneEditorGizmoHandle makeHandle(Vector2f pos) const;
		int updateHandles(const SceneEditorInputState& inputState);

		void setMode(PolygonGizmoMode mode);

		std::pair<Vertex, size_t> findInsertPoint(Vector2f pos) const;
	};
}
