/***
 * Draws the layers in a stack.
 */

#pragma once

#include "CartaLib/CartaLib.h"
#include "CartaLib/VectorGraphics/VGList.h"
#include "Data/Image/RenderRequest.h"
#include "Data/Image/RenderResponse.h"

#include <QString>
#include <QList>
#include <QObject>

#include <memory>

namespace Carta {
    namespace Lib {
        class LayeredRemoteVGView;
    }
}

namespace Carta {
namespace Data {

class Layer;


class DrawStackSynchronizer: public QObject {

    friend class Stack;

    Q_OBJECT

public:

    /**
     *  Constructor.
     *  @param view- the stack view.
     */
    DrawStackSynchronizer( Carta::Lib::LayeredRemoteVGView* view);

    /**
     * Return the client view size.
     * @return - the size of the stack view on the client.
     */
    QSize getClientSize() const;


    virtual ~DrawStackSynchronizer();

signals:
    /**
     * The view has been resized.
     */
    void viewResize();

private slots:

    /**
     * Repaint the image.
     */
    void _repaintFrameNow();

    /**
    * Notification that a stack layer has changed its image or vector graphics.
    */
    void _scheduleFrameRepaint( const std::shared_ptr<RenderResponse>& response );

private:

    void _render( QList<std::shared_ptr<Layer> >& datas,
            const std::shared_ptr<RenderRequest>& request );

    //Data View
    std::unique_ptr<Carta::Lib::LayeredRemoteVGView> m_view;
    QList< std::shared_ptr<Layer> > m_layers;
    QMap<QString, std::shared_ptr<RenderResponse> > m_images;
    bool m_repaintFrameQueued;

    int m_renderCount;
    int m_redrawCount;
    int m_selectIndex;

    DrawStackSynchronizer(const DrawStackSynchronizer& other);
    DrawStackSynchronizer& operator=(const DrawStackSynchronizer& other);

};

}
}
