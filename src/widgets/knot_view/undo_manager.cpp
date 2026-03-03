#include "undo_manager.hpp"

#include "commands.hpp"
#include "knot_view.hpp"

UndoManager::UndoManager(Knot_View* view, QObject* parent) : QObject(parent), m_view(view) {}

UndoManager::~UndoManager() = default;

void UndoManager::beginMacro(const QString& name)
{
    m_macroStack.push(new Knot_Macro(name, m_view, nullptr));
}

void UndoManager::endMacro()
{
    if (m_macroStack.empty()) return;
    Knot_Macro* macro = m_macroStack.pop();
    pushCommand(macro);
}

void UndoManager::pushCommand(Knot_Command* cmd)
{
    if (!m_macroStack.isEmpty())
        cmd->set_parent(m_macroStack.top());
    else
        m_undoStack.push(cmd);
}

void UndoManager::pushMacro(Knot_Macro* macro) { m_macroStack.push(macro); }

Knot_Macro* UndoManager::popMacro()
{
    if (m_macroStack.isEmpty()) return nullptr;
    return m_macroStack.pop();
}
