#ifndef UNDO_MANAGER_HPP
#define UNDO_MANAGER_HPP

#include <QStack>
#include <QString>
#include <QUndoStack>

class Knot_View;
class Knot_Command;
class Knot_Macro;

class UndoManager : public QObject
{
    Q_OBJECT

   public:
    explicit UndoManager(Knot_View* view, QObject* parent = nullptr);
    ~UndoManager();

    QUndoStack* stack() { return &m_undoStack; }

    void beginMacro(const QString& name);
    void endMacro();
    void pushCommand(Knot_Command* cmd);

    void pushMacro(Knot_Macro* macro);
    Knot_Macro* popMacro();

   private:
    Knot_View* m_view;
    QUndoStack m_undoStack;
    QStack<Knot_Macro*> m_macroStack;
};

#endif  // UNDO_MANAGER_HPP
