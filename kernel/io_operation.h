// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_IO_OPERATION_H
#define WAGTAIL_IO_OPERATION_H

#include <functional>
#include "kstream.h"

namespace wagtail
{
	class process;

	/** Base class for I/O operation classes. */
	class io_operation
	{
		public:
			/** Type specifying the status of an I/O operation. */
			enum class status
			{
				queued,           /**< The operation has been queued. */
				incomplete,       /**< The operation is in progress. */
				incomplete_ended, /**< The operation has ended, but did not complete in its entirety. */
				successful,       /**< The operation was successful. */
				failed            /**< The operation failed. */
			};

			/**
			 * Gets the status of the operation.
			 * @return the status of the operation.
			 */
			status get_status() const volatile { return stat; }

			/**
			 * Sets the status of the operation.
			 * @param stat the new status of the operation.
			 */
			void set_status(status stat) { this->stat = stat; }

			/**
			 * Gets the callback function for the I/O operation.
			 * @return the callback function for the I/O operation.
			 */
			std::function<void(io_operation *, status)> get_callback() const { return callback; }

			/**
			 * Sets the callback function for the I/O operation.
			 * @param callback the callback function for the I/O operation.
			 */
			void set_callback(std::function<void(io_operation *, status)> callback) { this->callback = callback; }

			/**
			 * Gets the owner of the I/O operation.
			 * @return a pointer to the process that initiated the I/O operation.
			 */
			process * get_owner() const { return owner; }

			/** Function called by the device when the operation has completed. */
			void complete(status stat);
		protected:
			/**
			 * Constructs a new I/O operation class.
			 * @param owner the process that initiated the I/O operation.
			 * @param callback function called when the operation completes.
			 */
			io_operation(process * owner = nullptr,
				std::function<void(io_operation *, status)> callback = nullptr);
		private:
			process * owner;
			std::function<void(io_operation *, status)> callback = nullptr;
			status stat = status::incomplete;
	};

	/**
	 * Operation class for reading from a device.
	 */
	class read_operation : public io_operation
	{
		public:
			/**
			 * Constructs a new read operation.
			 * @param destination the buffer to read data into.
			 * @param size the amount of data to read, in bytes.
			 * @param offset the address of the data to read, if needed for the underlying device.
			 * @param owner the process who owns the I/O operation. If this is specified, `destination`
			 *              is assumed to be a pointer into the specified process's address space.
			 * @param callback a callback function called when the I/O operation has completed.
			 */
			read_operation(void * destination, unsigned int size,
				unsigned int offset = 0, process * owner = nullptr,
				std::function<void(io_operation *, status)> callback = nullptr);

			/**
			 * Gets the offset to read from in the device.
			 * @return the offset to start reading from in the device.
			 */
			unsigned int get_offset() const { return offset; }

			/**
			 * Gets a pointer to the destination buffer.
			 * @return a pointer to the destination buffer.
			 */
			void * get_destination() const { return destination; }
			/**
			 * Gets a pointer to the destination buffer.
			 * @return a pointer to the destination buffer.
			 */
			void * get_destination() { return destination; }

			/**
			 * Gets the amount of data to read in the operation.
			 * @return the amount of data to be read, in bytes.
			 */
			unsigned int get_size() const { return size; }

			/**
			 * Gets the amount of data read in the operation.
			 * @return the amount of data read in the operation.
			 */
			unsigned int get_data_read() const { return data_read; }

			/**
			 * Sets the amount of data read in the operation.
			 * @param read the amount of data read in the operation.
			 */
			void set_data_read(unsigned int read) { data_read = read; }

			/**
			 * Adds to the amount of data read in the operation.
			 * @param add bytes to add to the total of data read.
			 */
			void add_data_read(unsigned int add) { data_read += add; }
		private:
			void * destination;
			unsigned int size, offset;
			unsigned int data_read = 0;
		friend kostream & operator<<(kostream & out, const read_operation & op);
	};
	kostream & operator<<(kostream & out, const read_operation & op);

	/**
	 * Operation class for writing to a device.
	 */
	class write_operation : public io_operation
	{
		public:
			/**
			 * Constructs a new write operation.
			 * @param source a pointer to the data to be written.
			 * @param size the amount of data to be written, in bytes.
			 * @param offset the address to store the data at, if needed by the underlying device.
			 * @param owner the process who owns the I/O operation. If this is specified, `source` is
			 *              assumed to be in the specified process's address space.
			 * @param callback a callback function called when the I/O operation has completed.
			 */
			write_operation(const void * source, unsigned int size, unsigned int offset = 0,
				process * owner = nullptr, std::function<void(io_operation *, status)> callback = nullptr);

			/**
			 * Gets a byte of data from the source buffer.
			 * @return a byte of data from the source buffer.
			 */
			unsigned char operator[](unsigned int index) const;

			/**
			 * Gets the size of the data to write.
			 * @return the number of bytes of data to write.
			 */
			unsigned int get_size() const { return size; }

			/**
			 * Gets a pointer to the source buffer.
			 * @return a pointer to the source buffer.
			 */
			const void * get_source() const { return source; }
		private:
			const void * source;
			unsigned int size, offset;
	};

	/**
	 * Operation class for reading one or more blocks from a block device.
	 */
	class block_read_operation final : public io_operation
	{
		public:
			block_read_operation(void * destination, unsigned int blocks, unsigned int address,
				process * owner = nullptr, std::function<void(io_operation *, status)> callback = nullptr);

			unsigned int get_blocks() const { return blocks; }
			unsigned int get_blocks_read() const { return blocks_read; }
			void add_blocks_read(unsigned int blocks) { blocks_read += blocks; }

			unsigned int get_address() const { return address; }
			void * get_destination() const { return destination; }
		private:
			void * destination;
			unsigned int blocks_read = 0, blocks, address;
		friend kostream & operator<<(kostream & out, const block_read_operation & op);
	};
	kostream & operator<<(kostream & out, const block_read_operation & op);

	/**
	 * Operation class for writing one or more blocks from a block device.
	 * @todo Implement this class.
	 */
	class block_write_operation final : public io_operation
	{
		public:
			block_write_operation(void * source, unsigned int blocks, unsigned int address,
				process * owner = nullptr, std::function<void(io_operation *, status)> callback = nullptr);
	};
}

#endif

