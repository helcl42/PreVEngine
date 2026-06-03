#include "HandTrackingComponent.h"

#ifdef ENABLE_XR

#include "../../render/mesh/MeshFactory.h"
#include "../../render/model/ModelFactory.h"

namespace prev_test::component::hand_tracking {
HandTrackingComponent::HandTrackingComponent(prev::core::device::Device& device)
{
    std::shared_ptr<prev_test::render::IMesh> mesh = prev_test::render::mesh::MeshFactory{}.CreateCube();
    m_jointModel = prev_test::render::model::ModelFactory{ device }.Create(mesh);
}

const prev::xr::HandsEvent& HandTrackingComponent::GetHandsData() const
{
    return m_handsData;
}

std::shared_ptr<prev_test::render::IModel> HandTrackingComponent::GetJointModel() const
{
    return m_jointModel;
}

void HandTrackingComponent::operator()(const prev::xr::HandsEvent& handsEvent)
{
    m_handsData = handsEvent;
}
} // namespace prev_test::component::hand_tracking

#endif
