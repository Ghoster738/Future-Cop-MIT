#ifndef MODEL_BUILDER_HEADER
#define MODEL_BUILDER_HEADER

#include "DataTypes.h"
#include <glm/gtc/quaternion.hpp>
#include <ostream>

namespace Utilities {

class ModelBuilder {
public:
    // GLTF names are the default names
    static const std::string POSITION_COMPONENT_NAME;
    static const std::string NORMAL_COMPONENT_NAME;
    static const std::string TANGENT_COMPONENT_NAME;
    static const std::string TEX_COORD_0_COMPONENT_NAME;
    static const std::string TEX_COORD_1_COMPONENT_NAME;
    static const std::string COLORS_0_COMPONENT_NAME;
    static const std::string JOINTS_INDEX_0_COMPONENT_NAME;
    static const std::string WEIGHTS_INDEX_0_COMPONENT_NAME;
    // If there is a component name that is specific to this application (or other programs) then name them beginning with "_".
    enum FilterType {
        NEAREST = 9728,
        LINEAR  = 9729,
        NEAREST_MIPMAP_NEAREST = 9984,
        LINEAR_MIPMAP_NEAREST  = 9985,
        NEAREST_MIPMAP_LINEAR  = 9986,
        LINEAR_MIPMAP_LINEAR   = 9987,
    };
    enum WrapType {
        CLAMP_TO_EDGE   = 33071,
        MIRRORED_REPEAT = 33648,
        REPEAT          = 10497
    };
    enum Target {
        ARRAY_BUFFER         = 34962,
        ELEMENT_ARRAY_BUFFER = 34963,
    };
    enum MeshPrimativeMode {
        POINTS         = 0,
        LINES          = 1,
        LINE_LOOP      = 2,
        LINE_STRIP     = 3,
        TRIANGLES      = 4,
        TRIANGLE_STRIP = 5,
        TRIANGLE_FAN   = 6
    };

    class VertexComponent {
    private:
        std::string name;
        unsigned int bit_field;
    public:
        VertexComponent( const char *const name );
        VertexComponent( const VertexComponent &copy );
        unsigned int begin; // This is where the first of the data begins.
        unsigned int stride; // This is the amount of data to be skipped.
        unsigned int size; // This is the size of the entire value.
        Utilities::DataTypes::Type type; // This states what kind of
        Utilities::DataTypes::ComponentType component_type; // The component type of the vertex type for example float int short ...
        const std::string& getName() const { return name; }
        bool isNormalized() const;
        void setNormalization( bool state );
        bool isPosition() const;
        bool isEqual( const VertexComponent &cmp ) const;
    };
    class TextureMaterial {
    public:
        TextureMaterial();
        uint32_t cbmp_resource_id;
        std::string file_name; // The file is relative to the texture.
        unsigned int starting_vertex_index; // The index of the starting vertices.
        unsigned int count; // The amount of vertices that the texture material covers.
        Utilities::DataTypes::Vec3Type min, max;
        
        // This holds the span for each material.
        std::vector< std::pair<Utilities::DataTypes::Vec3Type, Utilities::DataTypes::Vec3Type> > morph_bounds;
        
        void bounds( const TextureMaterial &material );
    };
    class Joint {
    public:
        Joint() : joint_r( nullptr ), joint_index( 0 ), position(), rotation() {}
        const Joint *joint_r; // The node offset to the joint.
        unsigned int joint_index;
        std::vector<glm::vec3> position;
        std::vector<glm::quat> rotation;
    };

    /**
    * This class is thrown if the vertex component does not exist.
    * Should only be called by addAttribute or addMorphAttribute
    */
    class InvalidVertexComponentIndex {
        public:
            InvalidVertexComponentIndex( unsigned int offending_index, bool is_morph );
            unsigned int offending_index; // The index that goes beyond either vertex_components or vertex_morph_components
            bool is_morph; // This is true if this exeception is called from
    };
    /**
    * This class is thrown if the vertex component parameter differs from the
    * Should only be called by addAttribute for now.
    */
    class NonMatchingVertexComponentTypes {
        public:
            NonMatchingVertexComponentTypes( unsigned int vertex_component_index,
                Utilities::DataTypes::ComponentType component_type, Utilities::DataTypes::ComponentType param_component_type,
                Utilities::DataTypes::Type type, Utilities::DataTypes::Type param_type );
            unsigned int vertex_component_index;
            Utilities::DataTypes::ComponentType component_type, param_component_type;
            Utilities::DataTypes::Type type, param_type;
    };
    /**
    * Due to the way the class works it forbids adding a vertex component while vertices can be written.
    * It is hard to expand interleaved vertices, so it is forbidden.
    */
    class CannotAddVertexComponentAfterSetup {};
    /**
    * Due to the way the class works it forbids adding a vertex component while vertices can be written.
    * It is hard to expand interleaved vertices, so it is forbidden.
    */
    class CannotAddVerticesWhenFinished {};
private:
    std::vector<uint32_t> primary_buffer;
    std::vector<VertexComponent> vertex_components;
    unsigned int total_components_size; // The size in uint32_t per vertex of the primary_buffer.

    std::vector<std::vector<uint32_t>> morph_frame_buffers;
    std::vector<VertexComponent> vertex_morph_components;
    unsigned int total_morph_components_size; // The size in uint32_t per vertex of the morph_frame_buffers elements.
    unsigned int vertex_morph_position_component_index;

    std::vector<TextureMaterial> texture_materials;

    unsigned int current_vertex_index;
    unsigned int vertex_amount;

    // This holds the bone transformations.
    // Its size indicates the number of frames avialable.
    std::vector<Joint> joints;
    unsigned int joint_amount;
    unsigned int joint_inverse_frame; // This value is greater than the joint_matrix_frames size then it was not set properly.

    bool is_model_finished; // This tells if the ModelBuilder should add more vertices.
    bool components_are_done; // This tells if the ModelBuilder should add more components.
    
    MeshPrimativeMode mesh_primative_mode;
public:
    ModelBuilder( MeshPrimativeMode mode = MeshPrimativeMode::TRIANGLES );
    ModelBuilder( const ModelBuilder& to_copy );
    ~ModelBuilder();

    /**
     * @param name The name of the component type if this is not a *ComponentName listed in this class you should make this name begin with "_" with all caps.
     * @param component_type The type of data the vector contains can be every value of the ComponentType.
     * @param type The amount of elements that are contained in the mathematical vector.
     * @param normalized If the componet type is an integer, then it repersents 1.0 - 0.0.
     * @return The index number of the vertex component.
     * @throw CannotAddVertexComponentAfterSetup If this method was called after setupVertexComponents.
     */
    unsigned int addVertexComponent( const std::string &name, Utilities::DataTypes::ComponentType component_type, Utilities::DataTypes::Type type, bool normalized = false );

    /**
     * @param name The name of the component type if this is not a *ComponentName listed in this class you should make this name begin with "_" with all caps.
     * @param component_type The type of data the vector contains can be every value of the ComponentType.
     * @param type The amount of elements that are contained in the mathematical vector.
     * @param normalized If the componet type is an integer, then it repersents 1.0 - 0.0.
     * @return The index number of the vertex component.
     * @throw CannotAddVertexComponentAfterSetup If this method was called after setupVertexComponents.
     */
    unsigned int addVertexComponent( const char *const name, Utilities::DataTypes::ComponentType component_type, Utilities::DataTypes::Type type, bool normalized = false );

    /**
     * This gets the number of vertex components that had been decleared.
     * @return the number of vertex components that exist in this class.
     */
    unsigned int getNumVertexComponents() const;

    /**
     * This method gets the vertex component. It was created, so one could use it for graphics.
     * @param vertex_component_index The index to the vertex component in std::vector<VertexType> vertex_components
     * @param element This gets a copy of all the parameters of the index.
     * @return If the parameter element has been written it will return true, so if it is out of bounds then it will return false.
     */
    bool getVertexComponent( unsigned int vertex_component_index, VertexComponent &element ) const;

    /**
     * This method sets the vertex component to be a morph target.
     * @param index The index to the vertex component in std::vector<VertexType> vertex_components, or the return value of addVertexComponent.
     * @throw CannotAddVertexComponentAfterSetup If this method was called after setupVertexComponents.
     * @return An index to the std::vector<VertexType> vertex_morph_components NOT to be confussed with vertex_components!
     */
    unsigned int setVertexComponentMorph( unsigned int vertex_component_index );

    /**
     * This gets the number of morph vertex components that had been decleared.
     * @return the number of morph vertex components that exist in this class.
     */
    unsigned int getNumMorphVertexComponents() const;

    /**
     * This method gets the morph vertex component. It was created, so one could use it for graphics.
     * @param vertex_morph_component_index The index to the vertex component in std::vector<VertexType> vertex_morph_components.
     * @param element This gets a copy of all the parameters of the morph component.
     * @return If the parameter element has been written it will return true, so if it is out of bounds then it will return false.
     */
    bool getMorphVertexComponent( unsigned int vertex_morph_component_index, VertexComponent &element ) const;
    
    
    /**
     * Allocate joints and the frames.
     * @param num_of_joints The amount of joints that this model will have.
     * @param num_of_frames The number of frames that this model will have. More specifically the number of frames for the skeletal animation.
    */
    void allocateJoints( unsigned int num_of_joints, unsigned int num_of_frames );
    
    /**
     * @return The number of joints that exists.
     */
    unsigned int getNumJoints() const;
    
    /**
     * @return The number of frames for the skeletal animation that exists.
     */
    unsigned int getNumJointFrames() const;
    
    MeshPrimativeMode getPrimativeMode() const;
    
    /**
     * This gets a copy of the matrix for the joint frame.
     * @param frame_index the frame index to the matrix joints.
     * @return either a direct pointer to the joint frames, or a nullptr if the frame_index is out of bounds or there are no joints.
     */
    glm::mat4 getJointFrame( unsigned int frame_index, unsigned int joint_index ) const;
    
    bool setJointParent( unsigned int joint_parent, unsigned joint_child );
    
    bool setJointFrame( unsigned int frame_index, unsigned int joint_index, const glm::vec3 &position, const glm::quat &rotation );

    /**
     * This tests to see if the any of the vertex components are invalid in glTF standards.
     * @param The index where the search would begin.
     * @param The stream output for warning.
     * @return It will return true if an error is found through vertex_components[begin] through vertex_components[end].
     */
    bool checkForInvalidComponent( int &begin, std::ostream *warning_output = nullptr ) const;

    /**
     * This should be called when the programmer is done adding all the components.
     * This sets up the VertexComponents, so they could write to the primary_buffer properly.
     *
     * @param morph_frames the amount of morphFrames there are.
     * @return If there is no vertex components in this class it will return false.
     * @throw CannotAddVertexComponentAfterSetup If this method was called twice!
     */
    bool setupVertexComponents( unsigned int morph_frames = 0 );

    /**
     * This method simply tells the class of how many vertices it will need.
     * It simply is there to make the mesh building process quicker.
     * @param size The amount of vertices to preallocate.
     * @throw CannotAddVerticesWhenFinished When this is called after the method finish.
     */
    void allocateVertices( unsigned int size );

    /**
     * This should be called when starting to write out the mesh or when the prievous material is done.
     * @param texture_index This is the index of the textures of the resource relative to the first texture.
     * @param cbmp_resource_id Set this to zero if not cbmp.
     * @return True if the setupVertexComponents() method was called.
     * @throw CannotAddVerticesWhenFinished When this is called after the method finish.
     */
    bool setMaterial( std::string file_name, uint32_t cbmp_resource_id = 0 );

    /**
     * This gets the number of materials for this class.
     * @return The number of materials in this class.
     */
    unsigned int getNumMaterials() const;

    /**
     * This gets the material from this class.
     * @param material_index The material index to access.
     * @param element The element to be received.
     * @return True if the element has successfully been written
     */
    bool getMaterial( unsigned int material_index, TextureMaterial &element ) const;

    /**
     * When all the methods of setVertexData and addMorphVertexData is called for the current vertex it is time to call this method.
     * This simply tells this class you are done with the current vertex.
     * @throw CannotAddVerticesWhenFinished When this is called after the method finish.
     */
    void startVertex();

    /**
     * This method sets the index of the vertices to the vertex_index parameter.
     *
     * Note: The index must have the advance method called on this index.
     * Important: the allocateVertices method has no effect for this method.
     * @param vertex_index The index to the vertex.
     * @return true if the index is not out of bounds. If this returns false then the index is not set.
     * @throw CannotAddVerticesWhenFinished When this is called after the method finish.
     */
    bool setVertexIndex( unsigned int vertex_index );

    /**
     * This adds the vertex data to the mesh.
     * @param vertex_component_index The index to the vertex_component which should be the return output from addVertexComponent.
     * @param data The data to be added to the vertex component.
     * @throw InvalidVertexComponent if the vertex_component_index goes beyond vertex_components.
     *
     * @throw NonMatchingVertexComponentTypes If the parameter data is not the same format as vertex_components[ vertex_component_index ].
     *
     * @throw CannotAddVerticesWhenFinished When this is called after the method finish.
     */
    void setVertexData( unsigned int vertex_component_index, const Utilities::DataTypes::DataType &data );

    /**
     * This adds the vertex data to the mesh.
     * TODO make this work for all of the subclasses of Utilities::DataTypes::DataType
     * TODO consider removing the original_value parameter, and make the code process the original_value in the primary_buffer.
     *
     * @param morph_vertex_component_index The morph vertex component to be used.
     * @param morph_frame_index The index of the morph_frame_buffers to modifiy the frame of the morph frame buffer.
     * @param original_value The previous value that was set by setVertexData for the
     * @param data
     * @throw InvalidVertexComponent if the morph_vertex_component_index goes beyond vertex_morph_components.
     *
     * @throw MorphFrameIndexOutOfBounds if the morph_frame_index goes beyond morph_frame_buffers
     *
     * @throw CannotAddVerticesWhenFinished When this is called after the method finish.
     */
    void addMorphVertexData( unsigned int morph_vertex_component_index, unsigned int morph_frame_index, const DataTypes::Vec3Type &original_value, const DataTypes::Vec3Type &data );

    /**
     * This gets the total amount of vertices that exist in this class.
     * @return the number of vertices which should be the sum of the vertices of all materials.
     */
    unsigned int getNumVertices() const;

    /**
     * This method gets the primary_buffer.
     *
     * @param size the size of this buffer in bytes.
     * @return A pointer to the primary_buffer.
     */
    void * getBuffer( unsigned int &size );
    
    /**
     * @return The number of Morph Frames of the model.
     */
    unsigned int getNumMorphFrames() const;

    /**
     * This method gets the morph_frame_buffers[morph_index] to the static model.
     *
     * @param morph_index the index of the
     * @param size the size of this buffer in bytes.
     * @return A pointer to the morph_frame_buffers[morph_index] or a nullptr.
     */
    void * getMorphBuffer( unsigned int morph_index, unsigned int &size );

    /**
     * This sets the class to be in a finished state. Meaning that adding more vertices or morph frames is forbidden.
     * It is actually optional to call this. It is primary there for conversions.
     * @note: If this method returns false then the class is not locked if setupVertexComponents was not called first.
     * @return true if setupVertexComponents was called before this method or if this method was called once.
     */
    bool finish();
    
    /**
     * This method is specialized for exporting bone animations.
     * This method transforms the position and normal vertices by the matrix.
     * This method also produces inverse matrices.
     * @warning be sure that the model had been finished first before using this method.
     * @return false if the model does not have the transformations. true if the model happens to have
     */
    bool applyJointTransforms( unsigned int frame_index );

    /**
     * This writes a glTF file.
     * @param file_path this holds the path to where it is going to write to.
     * @param title if this is not some empty string then this will give the model a name when exported.
     * @return true if the model is successfully written to the hard drive.
     */
    bool write( std::string file_path, std::string title = "" ) const;
    
    /**
     * Display the number of vertices, vertex types, morph types, and what not.
     * @param output this is the output stream.
     */
    void about( std::ostream &stream ) const;
    
    /**
     * This is the combine function to create a model with all of the vertices.
     * @param models must be a size of two in order for this program to work.
     * @return a pointer to a valid ModelBuilder, or nullptr if it has an error.
     */
    static ModelBuilder* combine( const std::vector<ModelBuilder*>& models, int & status );
    
    /**
     * This method makes a bounding sphere for the entire mesh.
     * @param position This holds the sphere's center location.
     * @param radius This holds the rotation of the sphere.
     * @return true if a bounding sphere is generated.
     */
    bool getBoundingSphere( glm::vec3 &position, float &rotation ) const;
};

}

#endif // MODEL_BUILDER_HEADER
