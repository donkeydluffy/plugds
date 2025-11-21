#include "extsystem/IComponent.h"

sss::extsystem::IComponent::~IComponent() = default;

auto sss::extsystem::IComponent::InitialiseEvent() -> void {}

auto sss::extsystem::IComponent::InitialisationFinishedEvent() -> void {}

auto sss::extsystem::IComponent::FinaliseEvent() -> void {}
