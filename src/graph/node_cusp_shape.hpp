/**
 * \file src/graph/node_cusp_shape.hpp
 * \brief API declarations for node cusp shape graph data structures and operations.
 */

#ifndef NODE_CUSP_SHAPE_HPP
#define NODE_CUSP_SHAPE_HPP
#include <QIcon>
#include <QMetaType>
#include <QString>

#include "path_builder.hpp"

struct Traversal_Info;
class Node_Style;

class Cusp_Shape
{
   public:
    virtual ~Cusp_Shape() {}

    /// (Translated) Human-readable name
    virtual QString name() const = 0;

    /// Machine-readable name, must be unique and constant for every derived class
    virtual QString machine_name() const = 0;

    /**
     *  \brief render the cusp to \c path
     *  \param[out] path    Output path builder
     *  \param      ti      Traversal information
     *  \param      style   Style to use
     */
    virtual void draw_joint(Path_Builder& path, const Traversal_Info& ti,
                            const Node_Style& style) const = 0;

    /// Icon to be shown in the UI
    virtual QIcon icon() const { return QIcon::fromTheme("cusp-other"); }

   protected:
    /**
        \brief get line pointing to the cusp endpoint
        \param start        starning handle line
        \param finish       ending handle line
        \param ti           traversal info
        \param def_dist     default distance from node (only if delta ~ 360)
    */
    QPointF cusp_point(const Traversal_Info& ti, double def_dist) const;

    /**
     * @brief Default path rendering (ie: not cusp)
     *
     *  Draws a cubic line from \c start to \c finish adjusting the handle size
     *  if they overlap
     *
     * @param[out]  path    Output path builder
     * @param       start   Start handle
     * @param       finish  Finish handle
     */
    void default_path(Path_Builder& path, QLineF start, QLineF finish) const;
};

Q_DECLARE_METATYPE(Cusp_Shape*)

class Cusp_Rounded : public Cusp_Shape
{
    QString name() const override { return QObject::tr("Round"); }

    QString machine_name() const override { return "round"; }

    virtual QIcon icon() const override { return QIcon::fromTheme("cusp-round"); }

    void draw_joint(Path_Builder& path, const Traversal_Info& ti,
                    const Node_Style& style) const override;
};
#endif  // NODE_CUSP_SHAPE_HPP
