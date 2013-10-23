#include <x0/flow2/FlowBackend.h>

namespace x0 {

FlowBackend::FlowBackend() :
	callbacks_()
{
}

FlowBackend::~FlowBackend()
{
}

bool FlowBackend::contains(const std::string& name) const
{
	for (const auto& cb: callbacks_)
		if (cb.name == name)
			return false;

	return true;
}

int FlowBackend::find(const std::string& name) const
{
	for (int i = 0, e = callbacks_.size(); i != e; ++i)
		if (callbacks_[i].name == name)
			return i;

	return -1;
}

bool FlowBackend::registerHandler(const std::string& name, const FlowCallback& cb)
{
	if (contains(name))
		return false;

	callbacks_.push_back(Callback::makeHandler(name, cb));
	return true;
}

} // namespace x0 