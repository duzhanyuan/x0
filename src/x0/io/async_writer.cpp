#include <x0/io/async_writer.hpp>
#include <x0/io/connection_sink.hpp>
#include <x0/connection.hpp>
#include <x0/types.hpp>
#include <ev++.h>
#include <memory>

namespace x0 {

class async_writer // {{{
{
private:
	std::shared_ptr<connection_sink> sink_;
	source_ptr source_;
	completion_handler_type handler_;
	std::size_t bytes_transferred_;

public:
	async_writer(std::shared_ptr<connection_sink> snk, const source_ptr& src, const completion_handler_type& handler) :
		sink_(snk),
		source_(src),
		handler_(handler),
		bytes_transferred_(0)
	{
	}

	~async_writer()
	{
		// unregister from connection's on_ready handler
		sink_->connection()->stop_io();
	}

public:
	static void write(const std::shared_ptr<connection_sink>& snk, const source_ptr& src, const completion_handler_type& handler)
	{
		async_writer *writer = new async_writer(snk, src, handler);
		writer->write();
	}

private:
	void callback(connection *)
	{
		write();
	}

	void write()
	{
#if !defined(NDEBUG)
		for (int i = 0; ; ++i)
#else
		for (;;)
#endif
		{
			ssize_t rv = sink_->pump(*source_); // true=complete,false=error,det=partial
			//DEBUG("writer(%p).pump: %ld; %s", this, rv, rv < 0 ? strerror(errno) : "");

			if (rv > 0)
			{
				DEBUG("async_writer(%p): write chunk done (%i)", this, i);
				// we wrote something (if not even all)
				bytes_transferred_ += rv;
			}
			else if (rv == 0)
			{
				DEBUG("async_writer(%p): write complete (%i)", this, i);
				// finished in success
				handler_(rv, bytes_transferred_);
				delete this;
				break;
			}
			else if (errno == EAGAIN || errno == EINTR)
			{
				DEBUG("async_writer(%p): write incomplete (EINT|EAGAIN) (%i)", this, i);
				// call back as soon as sink is ready for more writes
				sink_->connection()->on_ready(std::bind(&async_writer::callback, this, std::placeholders::_1), ev::WRITE);
				break;
			}
			else
			{
				DEBUG("async_writer(%p): write failed: %s (%i)", this, strerror(errno), i);
				// an error occurred
				handler_(errno, bytes_transferred_);
				delete this;
				break;
			}
		}
	}
}; // }}}

void async_write(connection *target, const source_ptr& src, const completion_handler_type& handler)
{
	async_write(std::make_shared<connection_sink>(target), src, handler);
}

void async_write(const std::shared_ptr<connection_sink>& snk, const source_ptr& src, const completion_handler_type& handler)
{
	async_writer::write(snk, src, handler);
}

} // namespace x0
