/**
 * \file src/widgets/knot_view/knot_view.cpp
 * \brief Implementation of knot view interactions in the knot view.
 */

#include "knot_view.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPrinter>
#include <QScrollBar>

#include "commands.hpp"
#include "context_menu_edge.hpp"
#include "context_menu_node.hpp"
#include "knot_graph_editor.hpp"
#include "knot_selection_controller.hpp"
#include "knot_view_io.hpp"
#include "knot_style_controller.hpp"
#include "knot_viewport_controller.hpp"
#include "resource_manager.hpp"
#include "undo_manager.hpp"
// #include <QGLWidget>

Knot_View::Knot_View(QString file)
    : mouse_mode(NO_MODE),
      m_file_name(file),
      paint_graph(true),
      m_fluid_refresh(true),
      context_menu_node(new Context_Menu_Node(this)),
      context_menu_edge(new Context_Menu_Edge(this)),
      active_tool(nullptr),
      tool_select(this, &m_graph),
      tool_edge_chain(this, &m_graph),
      tool_toggle_edge(this, &m_graph)
{
    m_undoManager = std::make_unique<UndoManager>(this, this);
    m_graph_editor = std::make_unique<Knot_Graph_Editor>(*this);
    m_io = std::make_unique<Knot_View_IO>(*this);
    m_selection_controller = std::make_unique<Knot_Selection_Controller>(*this);
    m_style_controller = std::make_unique<Knot_Style_Controller>(*this);
    m_viewport_controller = std::make_unique<Knot_Viewport_Controller>(*this);
    tool_edge_chain.set_graph(&m_graph);
    tool_select.set_graph(&m_graph);
    tool_toggle_edge.set_graph(&m_graph);
    active_tool = &tool_select;
    tool_select.enable(true);
    update_mouse_cursor();

    setWindowFilePath(file);
    setFrameStyle(StyledPanel | Plain);
    QGraphicsScene* scene = new QGraphicsScene;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setSceneRect(-width(), -height(), width() * 2, height() * 2);
    setScene(scene);
    setMouseTracking(true);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(NoAnchor);
    setResizeAnchor(AnchorViewCenter);

    scene->addItem(&m_graph);

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(update_scrollbars()));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(update_scrollbars()));

    QColor bg = QApplication::palette().color(QPalette::Highlight);
    rubberband.setPen(QPen(bg));
    bg.setAlpha(128);
    rubberband.setBrush(bg);
    rubberband.setZValue(100);

    connect(&m_grid, SIGNAL(grid_changed()), scene, SLOT(invalidate()));

    node_mover.add_handles_to_scene(scene);

    load_file(file);
}

Knot_View::~Knot_View() = default;

void Knot_View::copy_graph_style(const Graph& g) { m_style_controller->copy_graph_style(g); }

bool Knot_View::load_file(QIODevice& device, QString action_name)
{
    return m_io->load_file(device, action_name);
}

bool Knot_View::load_file(QString fname) { return m_io->load_file(fname); }

bool Knot_View::save_file(QString fname) { return m_io->save_file(fname); }

void Knot_View::set_knot_handle_lenght(double v) { m_style_controller->set_knot_handle_lenght(v); }

void Knot_View::set_knot_crossing_distance(double v)
{
    m_style_controller->set_knot_crossing_distance(v);
}

void Knot_View::set_knot_spacing(double v) { m_style_controller->set_knot_spacing(v); }

void Knot_View::set_knot_strand_count(int v) { m_style_controller->set_knot_strand_count(v); }

void Knot_View::set_knot_cusp_angle(double v) { m_style_controller->set_knot_cusp_angle(v); }

void Knot_View::set_knot_cusp_distance(double v) { m_style_controller->set_knot_cusp_distance(v); }

void Knot_View::set_knot_cusp_shape(Cusp_Shape* v) { m_style_controller->set_knot_cusp_shape(v); }

void Knot_View::set_knot_ege_slide(double v) { m_style_controller->set_knot_ege_slide(v); }

void Knot_View::set_selection_handle_lenght_nodes(double v)
{
    m_style_controller->set_selection_handle_lenght_nodes(v);
}

void Knot_View::set_selection_crossing_distance(double v)
{
    m_style_controller->set_selection_crossing_distance(v);
}

void Knot_View::set_selection_spacing(double v) { m_style_controller->set_selection_spacing(v); }

void Knot_View::set_selection_strand_count(int v)
{
    m_style_controller->set_selection_strand_count(v);
}

void Knot_View::set_selection_cusp_angle(double v) { m_style_controller->set_selection_cusp_angle(v); }

void Knot_View::set_selection_cusp_distance(double v)
{
    m_style_controller->set_selection_cusp_distance(v);
}

void Knot_View::set_selection_cusp_shape(Cusp_Shape* v)
{
    m_style_controller->set_selection_cusp_shape(v);
}

void Knot_View::set_selection_edge_type(Edge_Type* v) { m_style_controller->set_selection_edge_type(v); }

void Knot_View::set_selection_enabled_styles_nodes(Node_Style::Enabled_Styles v)
{
    m_style_controller->set_selection_enabled_styles_nodes(v);
}

void Knot_View::set_selection_handle_lenght_edges(double v)
{
    m_style_controller->set_selection_handle_lenght_edges(v);
}

void Knot_View::set_selection_enabled_styles_edges(Edge_Style::Enabled_Styles v)
{
    m_style_controller->set_selection_enabled_styles_edges(v);
}

void Knot_View::set_selection_edge_slide(double v) { m_style_controller->set_selection_edge_slide(v); }

void Knot_View::flip_horiz_selection() { m_selection_controller->flip_horiz_selection(); }

void Knot_View::flip_vert_selection() { m_selection_controller->flip_vert_selection(); }

void Knot_View::update_selection(bool select_edges)
{
    m_selection_controller->update_selection(select_edges);
}

void Knot_View::set_display_graph(bool enable)
{
    paint_graph = enable;

    for (Node* n : m_graph.nodes()) n->set_visible(enable);

    for (Edge* e : m_graph.edges()) e->set_visible(enable);

    scene()->invalidate();
}

void Knot_View::translate_view(QPointF delta) { m_viewport_controller->translate_view(delta); }

void Knot_View::translate_view_to(QPointF destination)
{
    m_viewport_controller->translate_view_to(destination);
}

Node* Knot_View::add_node(QPointF pos) { return m_graph_editor->add_node(pos); }

Node* Knot_View::add_breaking_node(QPointF pos) { return m_graph_editor->add_breaking_node(pos); }

Edge* Knot_View::add_edge(Node* n1, Node* n2) { return m_graph_editor->add_edge(n1, n2); }

void Knot_View::remove_edge(Edge* edge) { m_graph_editor->remove_edge(edge); }

void Knot_View::remove_node(Node* node) { m_graph_editor->remove_node(node); }

QUndoStack* Knot_View::undo_stack_pointer() { return m_undoManager->stack(); }

void Knot_View::begin_macro(QString name) { m_undoManager->beginMacro(name); }

void Knot_View::end_macro() { m_undoManager->endMacro(); }

void Knot_View::push_command(Knot_Command* cmd) { m_undoManager->pushCommand(cmd); }

QList<Node*> Knot_View::selected_nodes() const { return m_selection_controller->selected_nodes(); }

QList<Edge*> Knot_View::selected_edges() const { return m_selection_controller->selected_edges(); }

void Knot_View::zoom_view(double factor) { m_viewport_controller->zoom_view(factor); }

void Knot_View::set_zoom(double factor) { m_viewport_controller->set_zoom(factor); }

void Knot_View::set_active_tool(Knot_Tool& tool)
{
    if (active_tool != &tool) {
        active_tool->enable(false);
        active_tool = &tool;
        active_tool->enable(true);
        update_mouse_cursor();
    }
}

bool Knot_View::handle_mouse_press_on_scene(const QPoint& mouse_pos, const QPointF& scene_pos,
                                            const QPointF& snapped_scene_pos, QMouseEvent* event)
{
    if (active_tool->press(Mouse_Event(scene_pos, snapped_scene_pos, event))) return true;

    Transform_Handle* handle =
        dynamic_cast<Transform_Handle*>(scene()->itemAt(scene_pos, QTransform()));
    if (handle) {
        node_mover.set_nodes(selected_nodes());

        double anchor_angle = event->modifiers() & Qt::ShiftModifier;
        if (transform_mode() == Transform_Handle::SCALE) anchor_angle = !anchor_angle;

        node_mover.set_dragged_handle(handle, anchor_angle);
        set_mouse_mode(mouse_mode | DRAG_HANDLE);
        return true;
    }

    set_mouse_mode(mouse_mode | RUBBERBAND);
    rubberband.setPos(mapToScene(mouse_pos));
    rubberband.setRect(0, 0, 0, 0);
    scene()->addItem(&rubberband);
    return true;
}

QPointF Knot_View::handle_mouse_move_on_scene(const QPoint& mouse_pos, const QPointF& scene_pos,
                                              const QPointF& snapped_scene_pos, QMouseEvent* event)
{
    QPointF emitted_pos = scene_pos;

    if (event->buttons() & Qt::MiddleButton) {
        QPointF delta = mouse_pos - move_center;
        delta /= get_zoom_factor();
        translate_view(delta);
    } else if (mouse_mode & MOVE_BACK) {
        Node* node = node_at(scene_pos);
        Edge* edge = edge_at(scene_pos);
        if (mouse_mode & MOVE_GRID) {
            if (node)
                m_grid.set_origin(node->pos());
            else if (edge)
                m_grid.set_origin(edge->snap(scene_pos));
            else
                m_grid.set_origin(scene_pos);
        }
    } else if (mouse_mode & RUBBERBAND) {
        rubberband.setRect(
            QRectF(QPointF(0, 0), mapToScene(mouse_pos) - rubberband.pos()).normalized());
    } else if (mouse_mode & MOVE_NODES) {
        node_mover.set_pos(snapped_scene_pos);
        if (m_fluid_refresh) update_knot();
        emitted_pos = snapped_scene_pos;
    } else if (mouse_mode & DRAG_HANDLE) {
        node_mover.drag_handle(scene_pos, event->modifiers() & Qt::ControlModifier, m_grid.size());
        if (m_fluid_refresh) update_knot();
    } else {
        active_tool->move(Mouse_Event(scene_pos, snapped_scene_pos, event), emitted_pos);
    }

    return emitted_pos;
}

void Knot_View::handle_mouse_release_on_scene(const QPoint& mouse_pos, const QPointF& scene_pos,
                                              const QPointF& snapped_scene_pos, QMouseEvent* event)
{
    Q_UNUSED(mouse_pos)
    if (mouse_mode & RUBBERBAND) {
        scene()->removeItem(&rubberband);
        rubberband_select(nodes_in_rubberband(),
                          event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier));
        mouse_mode &= ~RUBBERBAND;
    } else if (mouse_mode & MOVE_NODES && event->button() != Qt::MiddleButton) {
        mouse_mode &= ~MOVE_NODES;
        node_mover.deploy(this, tr("Move Nodes"));
        update_knot();

        if (mouse_mode & EXTERNAL) {
            mouse_mode &= ~EXTERNAL;
            end_macro();
        }
    } else if (mouse_mode & DRAG_HANDLE) {
        node_mover.deploy(this, node_mover.mode() == Transform_Handle::ROTATE ? tr("Rotate Nodes")
                                                                              : tr("Scale Nodes"));
        set_mouse_mode(mouse_mode & ~DRAG_HANDLE);
    } else {
        active_tool->release(Mouse_Event(scene_pos, snapped_scene_pos, event));
    }
}

void Knot_View::handle_context_menu_request(const QPoint& mouse_pos, const QPointF& scene_pos)
{
    Node* node = node_at(scene_pos);
    Edge* edge = edge_at(scene_pos);
    if (node)
        context_menu_node->popup(node, mapToGlobal(mouse_pos));
    else if (edge)
        context_menu_edge->popup(edge, mapToGlobal(mouse_pos));
}

bool Knot_View::handle_wheel_zoom_or_transform(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->delta() < 0)
            zoom_view(0.8);
        else
            zoom_view(1.25);
        return true;
    }

    if (mouse_mode & MOVE_NODES) {
        if (event->modifiers() & Qt::ShiftModifier)
            node_mover.rotate(15 * (event->delta() > 0 ? 1 : -1));
        else
            node_mover.fixed_scale(event->delta() < 0, m_grid.size());
        update_knot();
        return true;
    }

    return false;
}

void Knot_View::handle_wheel_edge_type_or_scroll(QWheelEvent* event)
{
    QPointF scene_pos = mapToScene(event->pos());
    Edge* edge = edge_at(scene_pos);
    if (edge) {
        Edge_Type* next_type =
            event->delta() < 0 ? resource_manager().next_edge_type(edge->style().edge_type)
                               : resource_manager().prev_edge_type(edge->style().edge_type);
        push_command(new Change_Edge_Type(edge, edge->style().edge_type, next_type, this));
        return;
    }

    QScrollBar* scroll_bar = verticalScrollBar();
    int direction = event->delta() > 0 ? -1 : +1;
    if (event->modifiers() & Qt::ShiftModifier) scroll_bar = horizontalScrollBar();
    scroll_bar->setValue(scroll_bar->value() + direction * scroll_bar->singleStep());
}

void Knot_View::update_highlighted_item(const QPointF& scene_pos)
{
    for (QGraphicsItem* graphics_item : scene()->items()) {
        Graph_Item* item = dynamic_cast<Graph_Item*>(graphics_item);
        if (item) item->set_highlighted(false);
    }
    Graph_Item* item = item_at(scene_pos);
    if (item) item->set_highlighted(true);
}

void Knot_View::set_mode_edit_graph() { set_active_tool(tool_select); }

void Knot_View::set_mode_edge_chain() { set_active_tool(tool_edge_chain); }

void Knot_View::set_mode_toggle_edges() { set_active_tool(tool_toggle_edge); }

void Knot_View::set_mode_move_grid() { set_mouse_mode(mouse_mode | MOVE_GRID); }

void Knot_View::update_knot() { m_graph_editor->update_knot(); }

void Knot_View::set_knot_colors(const QList<QColor>& l) { m_style_controller->set_knot_colors(l); }

void Knot_View::set_knot_custom_colors(bool b) { m_style_controller->set_knot_custom_colors(b); }

void Knot_View::set_stroke_width(double w) { m_style_controller->set_stroke_width(w); }

void Knot_View::set_join_style(Qt::PenJoinStyle s) { m_style_controller->set_join_style(s); }

void Knot_View::set_brush_style(Qt::BrushStyle s) { m_style_controller->set_brush_style(s); }

void Knot_View::expand_scene_rect(int margin) { m_viewport_controller->expand_scene_rect(margin); }

void Knot_View::rubberband_select(QList<Node*> nodes, bool modifier)
{
    m_selection_controller->rubberband_select(nodes, modifier);
}

Node* Knot_View::node_at(QPointF p) const
{
    for (QGraphicsItem* item : scene()->items(p)) {
        if (Node* node = dynamic_cast<Node*>(item)) return node;
    }
    return nullptr;
}

Edge* Knot_View::edge_at(QPointF p) const
{
    for (QGraphicsItem* item : scene()->items(p)) {
        if (Edge* edge = dynamic_cast<Edge*>(item)) return edge;
    }
    return nullptr;
}

Graph_Item* Knot_View::item_at(QPointF p) const
{
    for (QGraphicsItem* item : scene()->items(p)) {
        if (Graph_Item* graph_item = dynamic_cast<Graph_Item*>(item)) return graph_item;
    }
    return nullptr;
}

bool Knot_View::insert(const Graph& graph, QString macro_name) { return m_io->insert(graph, macro_name); }

void Knot_View::set_fluid_refresh(bool enable)
{
    m_fluid_refresh = enable;
    if (enable) update_knot();
}

void Knot_View::set_antialiasing(bool enable) { setRenderHint(QPainter::Antialiasing, enable); }

bool Knot_View::has_antialiasing() const { return renderHints() & QPainter::Antialiasing; }

void Knot_View::view_fit() { m_viewport_controller->view_fit(); }

void Knot_View::reset_view() { m_viewport_controller->reset_view(); }

void Knot_View::mousePressEvent(QMouseEvent* event)
{
    QPoint mpos = event->pos();
    QPointF scene_pos = mapToScene(mpos);
    QPointF snapped_scene_pos = m_grid.nearest(scene_pos);

    if (event->button() == Qt::MiddleButton) {
        setCursor(Qt::ClosedHandCursor);
    } else if (mouse_mode & MOVE_BACK) {
        set_mouse_mode(mouse_mode & ~MOVE_BACK);
    } else if (!(mouse_mode & EXTERNAL)) {
        handle_mouse_press_on_scene(mpos, scene_pos, snapped_scene_pos, event);
    }
    move_center = mpos;
}

void Knot_View::mouseMoveEvent(QMouseEvent* event)
{
    QPoint mpos = event->pos();
    QPointF scene_pos = mapToScene(mpos);
    QPointF snapped_scene_pos = m_grid.nearest(scene_pos);
    QPointF emitted_pos = handle_mouse_move_on_scene(mpos, scene_pos, snapped_scene_pos, event);
    update_highlighted_item(scene_pos);

    move_center = mpos;
    scene()->invalidate();
    emit mose_position_changed(emitted_pos);
}

void Knot_View::mouseReleaseEvent(QMouseEvent* event)
{
    QPoint mpos = event->pos();
    QPointF scene_pos = mapToScene(mpos);
    QPointF snapped_scene_pos = m_grid.nearest(scene_pos);

    handle_mouse_release_on_scene(mpos, scene_pos, snapped_scene_pos, event);
    if (event->button() == Qt::RightButton) handle_context_menu_request(mpos, scene_pos);

    update_mouse_cursor();
}

void Knot_View::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPoint mpos = event->pos();
    QPointF scene_pos = mapToScene(mpos);
    QPointF snapped_scene_pos = m_grid.nearest(scene_pos);

    active_tool->double_click(Mouse_Event(scene_pos, snapped_scene_pos, event));
}

void Knot_View::wheelEvent(QWheelEvent* event)
{
    if (handle_wheel_zoom_or_transform(event)) return;
    handle_wheel_edge_type_or_scroll(event);
}

QList<Node*> Knot_View::nodes_in_rubberband() const
{
    return m_selection_controller->nodes_in_rubberband();
}

void Knot_View::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, backgroundBrush());
    m_grid.render(painter, rect);
}

void Knot_View::set_mouse_mode(Mouse_Mode mode)
{
    mouse_mode = mode;
    update_mouse_cursor();
}

void Knot_View::update_mouse_cursor()
{
    if (mouse_mode & MOVE_BACK)
        setCursor(Qt::SizeAllCursor);
    else if (mouse_mode & EXTERNAL)
        setCursor(Qt::DragCopyCursor);
    else if (mouse_mode & DRAG_HANDLE)
        setCursor(node_mover.current_handle_cursor());
    else if (mouse_mode & RUBBERBAND)
        setCursor(Qt::ArrowCursor);
    else
        setCursor(active_tool->cursor());
}

bool Knot_View::edit_graph_mode_enabled() const { return active_tool == &tool_select; }

bool Knot_View::edge_loop_mode_enabled() const { return active_tool == &tool_edge_chain; }

bool Knot_View::toggle_edges_mode_enabled() const { return active_tool == &tool_toggle_edge; }

void Knot_View::select_all() { m_selection_controller->select_all(); }

void Knot_View::select_connected() { m_selection_controller->select_connected(); }

void Knot_View::copy_selection() { m_io->copy_selection(); }

void Knot_View::paste_selection() { m_io->paste_selection(); }

void Knot_View::cut_selection() { m_io->cut_selection(); }

void Knot_View::snap_selection_to_grid() { m_selection_controller->snap_selection_to_grid(); }

void Knot_View::erase_selection() { m_selection_controller->erase_selection(); }

void Knot_View::connect_selection() { m_selection_controller->connect_selection(); }

void Knot_View::disconnect_selection() { m_selection_controller->disconnect_selection(); }

void Knot_View::merge_selection() { m_selection_controller->merge_selection(); }

void Knot_View::print(QPrinter* pr) { m_viewport_controller->print(pr); }
