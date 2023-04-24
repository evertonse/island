#include "vertex.hpp"

namespace cyx {

	// *------------------------------------------------------------------------------
	// | START	VertexArray																							
	// *------------------------------------------------------------------------------
	VertexArray::VertexArray() {
		// Generate Id for our VAO on gpu
		glLogCall(
			glGenVertexArrays(1, &_vertex_arrayID)
		// select this one
		);

		glBindVertexArray(_vertex_arrayID);
		std::cout << "[VertexArray]" << "created and bound\n";
	}
	
	auto VertexArray::add(VertexBuffer& vb , VertexLayout& layout)-> void  {
        this->bind();
		vb.bind();
		auto& attrs = layout.attrs();
        auto count = attrs.size();
		for (size_t i = 0; i < count; i++) {
			
			const auto& attr = attrs[i];

			std::cout << "[VertexArray]" << "Adding  VertexAttr: " << attr.to_string() <<"\n";
			
			u32 offset = attr.offset;

			glLogCall(
				glEnableVertexAttribArray(i);	
			);

			glLogCall(
				glVertexAttribPointer(
					i,
					attr.count(),
					VertexAttr::to_glenum(attr.type),
					attr.normalized ? GL_TRUE : GL_FALSE,
					layout.stride(),
					(const void*) attr.offset
				)
			);
            out << "i:" << i << '\n';
            out << "attrVout:" << attr.count() << '\n';
            out << "VertexAttr:" << VertexAttr::to_glenum_str(attr.type) << '\n';
            out << "attr.normlized?:" << (attr.normalized ? "GL_TRUE " : "GL_FALSE") << '\n';
            out << "layout.stride():" << layout.stride() << '\n';
            out << "(const void*) :" << attr.offset  << '\n';
            out << '\n';
		}
        vb.unbind();
        this->unbind();
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &_vertex_arrayID);
	}
    
	auto VertexArray::bind() const  -> void {

		std::cout << "[VertexArray]" << "bound\n";
		glBindVertexArray(_vertex_arrayID);
		_is_bound = true;
	}

	auto VertexArray::unbind() const -> void{

		glBindVertexArray(0);
		std::cout << "[VertexArray]" << "unbound\n";
		_is_bound = false;
	}

	// x------------------------------------------------------------------------------
	// | END VertexArray
	// x------------------------------------------------------------------------------


	
	// *------------------------------------------------------------------------------
	// | START	VertexAttr																							
	// *------------------------------------------------------------------------------
	auto VertexAttr::size() const -> u32 const 	 { return size_of_type(type);}
	auto VertexAttr::count() const -> u32 const  { return count_of_type(type);}

	// x------------------------------------------------------------------------------
	// | END
	// x------------------------------------------------------------------------------
	

	// *------------------------------------------------------------------------------
	// | START	VertexLayout																							
	// *------------------------------------------------------------------------------
		VertexLayout::VertexLayout() {};

		VertexLayout::VertexLayout(std::initializer_list<VertexAttr> attrs)
			: _attrs(attrs) 
		{
			calculate();
		}

		auto VertexLayout::attrs() -> const std::vector<VertexAttr>& { 
			calculate();
			return _attrs; 
		}
		
		auto VertexLayout::stride() const -> u32 { return _stride; }
		
		auto VertexLayout::push(VertexAttr& attr) -> void {
			this->_attrs.push_back(attr);
		}

		auto VertexLayout::push(std::initializer_list<VertexAttr> attrs) -> void {
			for (auto &&attr : attrs) {
				this->_attrs.push_back(attr);
			}
		}

		auto VertexLayout::push(VertexAttr&& attr) -> void {
			this->_attrs.push_back(attr);
		}

		auto VertexAttr::to_string() const -> const std::string {
			std::stringstream tostring;
			
			tostring 
				<< " Type: " << this->type
				<< " Size: " << this->size() 
				<< " Count: " << this->count() 
				<< " Offset: " << this->offset
				<< std::endl; 

			return tostring.str();
		}

		auto VertexLayout::calculate() -> void {
			size_t offset = 0;
			_stride = 0;
			
			for (auto& attr : _attrs) {
				attr.offset = offset;
                size_t size = attr.size(); 
				offset  += size;
				_stride += size;
			}
		}

	// x------------------------------------------------------------------------------
	// | END
	// x------------------------------------------------------------------------------

	// *------------------------------------------------------------------------------
	// | START	VertexBuffer																							
	// *------------------------------------------------------------------------------
	// size in bytes
	VertexBuffer::VertexBuffer(void* data, u32 size) {
		u32 vbo;// Generate Id for our VBO on gpu
		glGenBuffers(1, &vbo);
		std::cout << "[VertexBuffer]" << "created\n";
		set_data(data,size);
		_vertex_bufferID = vbo;

	}

	VertexBuffer::VertexBuffer() {
		u32 vbo;// Generate Id for our VBO on gpu
		glGenBuffers(1, &vbo);
		std::cout << "[VertexBuffer]" << "created  with no data yet\n";
		_vertex_bufferID = vbo;

	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &_vertex_bufferID);
	}

	auto VertexBuffer::set_data(void* data, u32 size)  -> void{
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
		std::cout << "[VertexBuffer]" << "data set\n";
	}

	auto VertexBuffer::bind()  -> void{
		glLogCall(
			glBindBuffer(GL_ARRAY_BUFFER, _vertex_bufferID);
		);
		std::cout << "[VertexBuffer]" << "bound\n";
		_is_bound = true;
	}

	auto VertexBuffer::unbind()  -> void{

		glLogCall(
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		);
		_is_bound = false;
		std::cout << "[VertexBuffer]" << "unbound\n";
	}

	// Only float for now
	auto VertexBuffer::
	// index : what index of this attr
	// count : how many floats represent this attr
	// stride_size : bytes needed to go to the second vertex
	// offset_size : bytes needed to go from the start of the first attr to this attr

	/*
	Example:
	Vertex array = [x=0.1,y=0.2, r=255,g=255,b=255 ]
	attr = position(index=0,count=2,stride=sizeof(float)*5,0)
	attr = color(index=1,count=3,stride=sizeof(float)*5,sizeof(float)*2)
	*/
	set_attr(u32 index, i32 count, i32 stride_size, i32 offset_size) -> void {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(
			index					/* from the index 'location'			*/,
			count        	/* count of elements						*/,
			GL_FLOAT 			/* Type of each data is only float	*/,
			GL_FALSE 			/* Not normalized                  	*/,
			stride_size      	/* If other attrib, so  stride   	*/,
			(const void*)offset_size
		);

	}
	// x------------------------------------------------------------------------------
	// | END VertexBuffer
	// x------------------------------------------------------------------------------




	// *------------------------------------------------------------------------------
	// | START	VertexIndex																							
	// *------------------------------------------------------------------------------
	VertexIndex::VertexIndex() {
		u32 vib;// Generate Id for our VBO on gpu
		glGenBuffers(1, &vib);
		std::cout << "[VertexIndex]" << "created\n";
		this->_vertex_indexID = vib;
	}

	VertexIndex::~VertexIndex() {
	}

	// data : HAS to be UNSIGNED
	//// TODO(Everton): ""change ir to take count instead of size"""
	auto VertexIndex::set_data(void* data, u32 size) -> void {
		bind();
		set_count(u32(size / sizeof(u32)));
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
		std::cout << "[VertexIndex]" << "data set\n";
	}

	auto VertexIndex::bind() const -> void {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_vertex_indexID);
		std::cout << "[VertexIndex]" << "bound\n";
		_is_bound = true;
	
	}

	auto VertexIndex::count() const -> u32 {
		return this->_count;
	}

	auto VertexIndex::set_count(u32 count) -> void {
		this->_count = count;
	}

	auto VertexIndex::unbind() const -> void {
		_is_bound = false;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		std::cout << "[VertexIndex]" << "unbound\n";
	}
}
