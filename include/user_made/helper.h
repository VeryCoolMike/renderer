
struct ray_cast
{
    object hit;
    glm::vec3 pos;
    bool valid;
};

const float EPSILON = 0.000001f;

ray_cast raycast(glm::vec3 origin, glm::vec3 direction){
    float closest_intersection = std::numeric_limits<float>::max(); // Largest possible number (Technical limit)
    bool found_intersection = false;
    object intersection_object;

    for (int i = 0; i < objects.size(); i++) // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    {
        if (objects[i].name == "test")
        {
            continue;
        }
        for (int v = 0; v < objects[i].vertices.size(); v += 24) // 8 is the vertex size but we need a vertice so 3 vertexes (8*3)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, objects[i].transform.pos);
            model = glm::scale(model, glm::vec3(objects[i].transform.scale));

            glm::vec3 V0 = glm::vec3(model * glm::vec4(objects[i].vertices[v], objects[i].vertices[v+1], objects[i].vertices[v+2], 1.0f)); // Vertex 1
            glm::vec3 V1 = glm::vec3(model * glm::vec4(objects[i].vertices[v+8], objects[i].vertices[v+9], objects[i].vertices[v+10], 1.0f)); // Vertex 2
            glm::vec3 V2 = glm::vec3(model * glm::vec4(objects[i].vertices[v+16], objects[i].vertices[v+17], objects[i].vertices[v+18], 1.0f)); // Vertex 3

            glm::vec3 edge1 = V1 - V0; // e1
            glm::vec3 edge2 = V2 - V0; // e2

            glm::vec3 cross_product = glm::cross(direction, edge2); // h

            float dot_product = glm::dot(edge1, cross_product); // a

            if (dot_product > -EPSILON && dot_product < EPSILON)
            {
                //std::cout << "1" << std::endl;
                continue;
            }

            float intersection_param = 1.0f / dot_product; // f

            glm::vec3 vector_s = origin - V0; // s

            float dot_product2 = intersection_param * glm::dot(vector_s, cross_product); // u
            
            if (dot_product2 < 0 || dot_product2 > 1) // Outside of triangle
            {
                //std::cout << "2" << std::endl;
                continue;
            }

            glm::vec3 cross_product2 = glm::cross(vector_s, edge1); // q

            float dot_product3 = intersection_param * glm::dot(direction, cross_product2); // v

            if (dot_product3 < 0.0f || dot_product2 + dot_product3 > 1.0f)
            {
                //std::cout << "3" << std::endl;
                continue;
            }

            float intersection_point = intersection_param * glm::dot(edge2, cross_product2); // t

            if (intersection_point > EPSILON && intersection_point < closest_intersection)
            {
                closest_intersection = intersection_point;
                found_intersection = true;
                intersection_object = objects[i];
                
            }
        }
    }

    if (found_intersection)
    {
        glm::vec3 intersectionPos = origin + closest_intersection * direction;

        ray_cast finished_cast;
        finished_cast.hit = intersection_object;
        finished_cast.pos = intersectionPos;
        finished_cast.valid = true;
        return finished_cast;
    }
    else
    {
        ray_cast finished_cast;
        finished_cast.valid = false;
        return finished_cast;
    }
}