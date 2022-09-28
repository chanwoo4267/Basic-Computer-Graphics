#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION 0

#define NUMBER_OF_POINTS 1
GLuint points_VBO, points_VAO;
GLfloat points_vertices[NUMBER_OF_POINTS][3] = { { 0.0f, 0.0f, 0.0f } };

void prepare_points(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &points_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, points_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_vertices), &points_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &points_VAO);
	glBindVertexArray(points_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, points_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_points(float r, float g, float b) {
	glBindVertexArray(points_VAO);
	glUniform3f(loc_primitive_color, r, g, b);
	glDrawArrays(GL_POINTS, 0, NUMBER_OF_POINTS);
	glBindVertexArray(0);
}

GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_axes(void) {
	glBindVertexArray(axes_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}

#define N_OBJECTS 10
#define OBJECT_SQUARE16 0
#define OBJECT_TIGER 1
#define OBJECT_COW 2
#define OBJECT_DRAGON 3
#define OBJECT_BIKE 4
#define OBJECT_BUS 5
#define OBJECT_GODZILLA 6
#define OBJECT_IRONMAN 7
#define OBJECT_OPTIMUS 8
#define OBJECT_TANK 9

GLuint object_VBO[N_OBJECTS], object_VAO[N_OBJECTS];
int object_n_triangles[N_OBJECTS];
GLfloat *object_vertices[N_OBJECTS];

int read_triangular_mesh(GLfloat **object, int bytes_per_primitive, char *filename) {
	int n_triangles;
	FILE *fp;

	fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL){
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles*bytes_per_primitive);
	if (*object == NULL){
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void set_up_object(int object_ID, char *filename, int n_bytes_per_vertex) {
	object_n_triangles[object_ID] = read_triangular_mesh(&object_vertices[object_ID],
		3 * n_bytes_per_vertex, filename);
	// Error checking is needed here.

	// Initialize vertex buffer object.
	glGenBuffers(1, &object_VBO[object_ID]);

	glBindBuffer(GL_ARRAY_BUFFER, object_VBO[object_ID]);
	glBufferData(GL_ARRAY_BUFFER, object_n_triangles[object_ID] * 3 * n_bytes_per_vertex,
		object_vertices[object_ID], GL_STATIC_DRAW);

	// As the geometry data exists now in graphics memory, ...
	free(object_vertices[object_ID]);

	// Initialize vertex array object.
	glGenVertexArrays(1, &object_VAO[object_ID]);
	glBindVertexArray(object_VAO[object_ID]);

	glBindBuffer(GL_ARRAY_BUFFER, object_VBO[object_ID]);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_square(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_SQUARE16, "Data/Square16_triangles_vnt.geom", 8 * sizeof(float));

	material_floor.ambient_color[0] = 0.0f;
	material_floor.ambient_color[1] = 0.1f;
	material_floor.ambient_color[2] = 0.0f;
	material_floor.ambient_color[3] = 1.0f;

	material_floor.diffuse_color[0] = 0.2f;
	material_floor.diffuse_color[1] = 0.3f;
	material_floor.diffuse_color[2] = 0.4f;
	material_floor.diffuse_color[3] = 1.0f;

	material_floor.specular_color[0] = 0.5f;
	material_floor.specular_color[1] = 0.3f;
	material_floor.specular_color[2] = 0.4f;
	material_floor.specular_color[3] = 1.0f;

	material_floor.specular_exponent = 3.2f;

	material_floor.emissive_color[0] = 0.0f;
	material_floor.emissive_color[1] = 0.0f;
	material_floor.emissive_color[2] = 0.0f;
	material_floor.emissive_color[3] = 1.0f;
}

void prepare_tiger(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_TIGER, "Data/Tiger_00_triangles_vnt.geom", 8 * sizeof(float));

	// init material parameter
	material_tiger.ambient_color[0] = 0.3725f;
	material_tiger.ambient_color[1] = 0.295f;
	material_tiger.ambient_color[2] = 0.545f;
	material_tiger.ambient_color[3] = 1.0f;

	material_tiger.diffuse_color[0] = 0.55164f;
	material_tiger.diffuse_color[1] = 0.20648f;
	material_tiger.diffuse_color[2] = 0.82648f;
	material_tiger.diffuse_color[3] = 1.0f;

	material_tiger.specular_color[0] = 0.58281f;
	material_tiger.specular_color[1] = 0.45802f;
	material_tiger.specular_color[2] = 0.76065f;
	material_tiger.specular_color[3] = 1.0f;

	material_tiger.specular_exponent = 41.2f;

	material_tiger.emissive_color[0] = 0.1f;
	material_tiger.emissive_color[1] = 0.1f;
	material_tiger.emissive_color[2] = 0.1f;
	material_tiger.emissive_color[3] = 1.0f;
}

void prepare_cow(void) {
	// 3 = 3 for vertex
	set_up_object(OBJECT_COW, "Data/Cow_triangles_v.geom", 3 * sizeof(float));
}

void prepare_dragon(void) {
	set_up_object(OBJECT_DRAGON, "Data/dragon_vnt.geom", 8 * sizeof(float));
}

void prepare_bike(void) {
	set_up_object(OBJECT_BIKE, "Data/bike_vnt.geom", 8 * sizeof(float));
}

void prepare_bus(void) {
	set_up_object(OBJECT_BUS, "Data/bus_vnt.geom", 8 * sizeof(float));

	// init material parameter
	material_bus.ambient_color[0] = 0.31725f;
	material_bus.ambient_color[1] = 0.695f;
	material_bus.ambient_color[2] = 0.0635f;
	material_bus.ambient_color[3] = 1.0f;

	material_bus.diffuse_color[0] = 0.65164f;
	material_bus.diffuse_color[1] = 0.48648f;
	material_bus.diffuse_color[2] = 0.39648f;
	material_bus.diffuse_color[3] = 1.0f;

	material_bus.specular_color[0] = 0.728281f;
	material_bus.specular_color[1] = 0.355802f;
	material_bus.specular_color[2] = 0.666065f;
	material_bus.specular_color[3] = 1.0f;

	material_bus.specular_exponent = 41.2f;

	material_bus.emissive_color[0] = 0.1f;
	material_bus.emissive_color[1] = 0.1f;
	material_bus.emissive_color[2] = 0.1f;
	material_bus.emissive_color[3] = 1.0f;
}

void prepare_godzilla(void) {
	set_up_object(OBJECT_GODZILLA, "Data/godzilla_vnt.geom", 8 * sizeof(float));
}

void prepare_ironman(void) {
	set_up_object(OBJECT_IRONMAN, "Data/ironman_vnt.geom", 8 * sizeof(float));
}

void prepare_optimus(void) {
	set_up_object(OBJECT_OPTIMUS, "Data/optimus_vnt.geom", 8 * sizeof(float));
}

void prepare_tank(void) {
	set_up_object(OBJECT_TANK, "Data/tank_vnt.geom", 8 * sizeof(float));
}

void draw_object(int object_ID, float r, float g, float b) {
	glFrontFace(GL_CW);
	glUniform3f(loc_primitive_color, r, g, b);
	glBindVertexArray(object_VAO[object_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * object_n_triangles[object_ID]);
	glBindVertexArray(0);
}

void draw_object_simple(int object_ID)
{
	glFrontFace(GL_CW);

	glBindVertexArray(object_VAO[object_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * object_n_triangles[object_ID]);
	glBindVertexArray(0);
}



