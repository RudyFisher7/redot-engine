#include "theta_star_3d.h"


namespace ThetaStar {


ThetaStar3D::ThetaStar3D() { // todo:: enforce abstractness
    //ERR_FAIL_MSG("ThetaStar3D can't be created directly. It is an abstract class.");
}


ThetaStar3D::ThetaStar3D(Vector3i dimensions): _dimensions(dimensions) { // todo:: enforce abstractness
}


ThetaStar3D::~ThetaStar3D() {
    clear();
}


void ThetaStar3D::reserve(const uint32_t new_capacity) {
    _points.reserve(new_capacity);
}


void ThetaStar3D::clear() {
    for (OAHashMap<int64_t, Point<Vector3i>*>::Iterator it = _points.iter(); it.valid; it = _points.next_iter(it)) {
		memdelete(*(it.value));
	}

    _points.clear();
	_is_dirty = false;
}


Vector3i ThetaStar3D::get_dimensions() const {
    return _dimensions;
}


int64_t ThetaStar3D::get_size() const {
    int64_t result = 0;
    result = static_cast<int64_t>(_dimensions.x);
    result *= static_cast<int64_t>(_dimensions.y);
    result *= static_cast<int64_t>(_dimensions.z);
    return result;
}


bool ThetaStar3D::is_line_of_sight_check_enabled() const {
	return _is_line_of_sight_check_enabled;
}


int64_t ThetaStar3D::get_point_count() const {
    return static_cast<int64_t>(_points.get_num_elements());
}

int64_t ThetaStar3D::get_disabled_point_count() const {
    return _number_of_disabled_points;
}


bool ThetaStar3D::is_empty() const {
    return _points.is_empty();
}


int64_t ThetaStar3D::get_capacity() const {
    return static_cast<int64_t>(_points.get_capacity());
}


int64_t ThetaStar3D::get_point_id(const Vector3i position) {
    int64_t result = -1;
    int64_t id = -1;

    id = get_point_hash(position);

    if (_points.has(id)) {
        result = id;
    }

    return result;
}


Vector3i ThetaStar3D::get_point_position(const int64_t id) const {
    Vector3i result;
    Point<Vector3i>* point = nullptr;

    if (_points.lookup(id, point)) {
        result = point->position;
    } else {
        result = Vector3i(-1, -1, -1); //todo:: this is not a valid error return case if overridden in GDScript
    }

    return result;
}


int64_t ThetaStar3D::get_point_hash(const Vector3i position) {
    int64_t result = -1;
    bool is_position_valid = false;

    is_position_valid = _is_position_valid(position, true);
    if (is_position_valid) {
        result = _hash_position(position);
    }

    return result;
}


bool ThetaStar3D::is_position_valid_for_hashing(const Vector3i position) const {
    return _is_position_valid(position);
}


bool ThetaStar3D::is_id_disabled(const int64_t id) const {
    bool result = false;

    Point<Vector3i>* point = nullptr;

    result = _points.lookup(id, point);

    if (result) {
        result = !point->enabled;
    }

    return result;
}


bool ThetaStar3D::is_position_disabled(const Vector3i position) {
    bool result = false;

    int64_t id = _hash_position(position);

    result = is_id_disabled(id);

    return result;
}


bool ThetaStar3D::has_id(const int64_t id) {
    bool result = false;

    result = _points.has(id);

    return result;
}


bool ThetaStar3D::has_point(const Vector3i position) {
    bool result = false;
    int64_t id = _hash_position(position);

    result = _points.has(id);

    return result;
}


TypedArray<Vector3i> ThetaStar3D::get_points() const {
    TypedArray<Vector3i> vectors;

    for (OAHashMap<int64_t, Point<Vector3i>*>::Iterator it = _points.iter(); it.valid; it = _points.next_iter(it)) {
		vectors.push_back((*(it.value))->position);
	}

    return vectors;
}


TypedArray<Vector3i> ThetaStar3D::get_point_connections(const Vector3i position) {
    TypedArray<Vector3i> vectors;
    Point<Vector3i>* point = nullptr;
    int64_t id = _hash_position(position);

    if (_points.lookup(id, point)) {
        for (OAHashMap<int64_t, Point<Vector3i>*>::Iterator it = point->neighbors.iter(); it.valid; it = point->neighbors.next_iter(it)) {
            vectors.push_back((*(it.value))->position);
        }
    }

    return vectors;
}


PackedInt64Array ThetaStar3D::get_id_path_from_positions(const Vector3i from, const Vector3i to) {
    PackedInt64Array result;
    LocalVector<const Point<Vector3i>*> result_points;
    bool is_from_valid = false;
    bool is_to_valid = false;
    int64_t from_id = 0;
    int64_t to_id = 0;
    Point<Vector3i>* from_point = nullptr;
    Point<Vector3i>* to_point = nullptr;

    is_from_valid = _is_position_valid(from, true);
    is_to_valid = _is_position_valid(to, true);

    if (is_from_valid && is_to_valid) {
        from_id = _hash_position(from);
        to_id = _hash_position(to);

        if (_points.lookup(from_id, from_point) && _points.lookup(to_id, to_point)) {
            _get_point_path(from_point, to_point, result_points);
        }
    }

    for (LocalVector<const Point<Vector3i>*>::Iterator it = result_points.begin(); it != result_points.end(); ++it) {
        result.push_back((*it)->id);
    }

    return result;
}


TypedArray<Vector3i> ThetaStar3D::get_point_path_from_positions(const Vector3i from, const Vector3i to) {
    TypedArray<Vector3i> result;
    LocalVector<const Point<Vector3i>*> result_points;
    bool is_from_valid = false;
    bool is_to_valid = false;
    int64_t from_id = 0;
    int64_t to_id = 0;
    Point<Vector3i>* from_point = nullptr;
    Point<Vector3i>* to_point = nullptr;

    is_from_valid = _is_position_valid(from, true);
    is_to_valid = _is_position_valid(to, true);

    if (is_from_valid && is_to_valid) {
        from_id = _hash_position(from);
        to_id = _hash_position(to);

        if (_points.lookup(from_id, from_point) && _points.lookup(to_id, to_point)) {
            _get_point_path(from_point, to_point, result_points);
        }
    }

    for (LocalVector<const Point<Vector3i>*>::Iterator it = result_points.begin(); it != result_points.end(); ++it) {
        result.push_back((*it)->position);
    }

    return result;
}


PackedInt64Array ThetaStar3D::get_id_path_from_ids(const int64_t from, const int64_t to) {
    PackedInt64Array result;
    LocalVector<const Point<Vector3i>*> result_points;
    Point<Vector3i>* from_point = nullptr;
    Point<Vector3i>* to_point = nullptr;

    if (_points.lookup(from, from_point) && _points.lookup(to, to_point)) {
        _get_point_path(from_point, to_point, result_points);
    }

    for (LocalVector<const Point<Vector3i>*>::Iterator it = result_points.begin(); it != result_points.end(); ++it) {
        result.push_back((*it)->id);
    }

    return result;
}


TypedArray<Vector3i> ThetaStar3D::get_point_path_from_ids(const int64_t from, const int64_t to) {
    TypedArray<Vector3i> result;
    LocalVector<const Point<Vector3i>*> result_points;
    Point<Vector3i>* from_point = nullptr;
    Point<Vector3i>* to_point = nullptr;

    if (_points.lookup(from, from_point) && _points.lookup(to, to_point)) {
        _get_point_path(from_point, to_point, result_points);
    }

    for (LocalVector<const Point<Vector3i>*>::Iterator it = result_points.begin(); it != result_points.end(); ++it) {
        result.push_back((*it)->position);
    }

    return result;
}


TypedArray<Vector3> ThetaStar3D::get_point_path_from_off_graph_positions(const Vector3 from, const Vector3 to) {
    TypedArray<Vector3> result;
    LocalVector<const Point<Vector3i>*> result_points;
    bool is_from_valid = false;
    bool is_to_valid = false;
    int64_t from_id = 0;
    int64_t to_id = 0;
    Point<Vector3i>* from_point = nullptr;
    Point<Vector3i>* to_point = nullptr;

    from_point = _get_closest_point_toward(from, to);
    to_point = _get_closest_point_toward(to, from);

    is_from_valid = _is_position_valid(from, true);
    is_to_valid = _is_position_valid(to, true);

    if (is_from_valid && is_to_valid) {
        from_id = _hash_position(from);
        to_id = _hash_position(to);

        if (_points.lookup(from_id, from_point) && _points.lookup(to_id, to_point)) {
            _get_point_path(from_point, to_point, result_points);
        }
    }

    for (LocalVector<const Point<Vector3i>*>::Iterator it = result_points.begin(); it != result_points.end(); ++it) {
        result.push_back(Vector3((*it)->position));
    }

    if (!result.is_empty()) {
        result.insert(0, from);
        result.insert(result.size() - 1, to);
    }

    return result;
}


void ThetaStar3D::set_dimensions(const Vector3i dimensions) {
	_dimensions = dimensions;

	if (!_points.is_empty()) {
		_is_dirty = true;
	}
}


void ThetaStar3D::enable_line_of_sight_check(bool value) {
	_is_line_of_sight_check_enabled = value;
}


bool ThetaStar3D::add_point(const Vector3i position) {
    bool result = false;

    result = _is_position_valid(position, true);

    if (result) {
        int64_t id = _hash_position(position);

    	if (has_id(id)) {
    		Vector3i pos = get_point_position(id);
    		int i = 0;
    	}

        Point<Vector3i>* point = memnew(Point<Vector3i>);

        point->id = id;
        point->position = position;

        _points.set(id, point);
    }

    return result;
}


bool ThetaStar3D::remove_point(const Vector3i position) {
    bool result = false;

    result = _is_position_valid(position);

    if (result) {
        int64_t id = _hash_position(position);

        Point<Vector3i>* point = nullptr;
        result = _points.lookup(id, point);

        if (result) {
            for (OAHashMap<int64_t, Point<Vector3i>*>::Iterator it = point->neighbors.iter(); it.valid; it = point->neighbors.next_iter(it)) {
                (*it.value)->neighbors.remove(id);
            }

            memdelete(point);
            _points.remove(id);
        }
    }

    return result;
}


bool ThetaStar3D::disable_point_by_position(const Vector3i position, const bool disable) {
    bool result = _is_position_valid(position, true);
    if (result) {
        result = disable_point_by_id(_hash_position(position));
    }

    return result;
}


bool ThetaStar3D::disable_point_by_id(const int64_t id, const bool disable) {
    bool result = false;
    Point<Vector3i>* point = nullptr;

    if (_points.lookup(id, point)) {

        if (disable && point->enabled) {
            _number_of_disabled_points++;
        } else if (!disable && !point->enabled) {
            _number_of_disabled_points--;
        }

        point->enabled = !disable;

        result = true;
    }

    return result;
}


bool ThetaStar3D::connect_points(const Vector3i from, const Vector3i to, const bool bidirectional) {
    bool result = false;

    bool is_from_valid = _is_position_valid(from, true);
    bool is_to_valid = _is_position_valid(to, true);

    result = is_from_valid && is_to_valid;

    if (result) {
        int64_t from_id = _hash_position(from);
        int64_t to_id = _hash_position(to);

        result = _connect_points(from_id, to_id, bidirectional);
    }

    return result;
}


bool ThetaStar3D::disconnect_points(const Vector3i from, const Vector3i to, const bool bidirectional) {
    bool result = false;

    int64_t from_id = _hash_position(from);
    int64_t to_id = _hash_position(to);

    Point<Vector3i>* from_point = nullptr;
    Point<Vector3i>* to_point = nullptr;
    result = _points.lookup(from_id, from_point) && _points.lookup(to_id, to_point);

    if (result) {
        if (from_point->neighbors.has(to_id)) {
            from_point->neighbors.remove(to_id);
        }
    }

    if (bidirectional) {
        if (to_point->neighbors.has(from_id)) {
            to_point->neighbors.remove(from_id);
        }
    }

    return result;
}


void ThetaStar3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("reserve", "new_capacity"), &ThetaStar3D::reserve);
    ClassDB::bind_method(D_METHOD("clear"), &ThetaStar3D::clear);
    ClassDB::bind_method(D_METHOD("get_dimensions"), &ThetaStar3D::get_dimensions);
    ClassDB::bind_method(D_METHOD("get_size"), &ThetaStar3D::get_size);
    ClassDB::bind_method(D_METHOD("is_line_of_sight_check_enabled"), &ThetaStar3D::is_line_of_sight_check_enabled);
    ClassDB::bind_method(D_METHOD("get_point_count"), &ThetaStar3D::get_point_count);
    ClassDB::bind_method(D_METHOD("get_disabled_point_count"), &ThetaStar3D::get_disabled_point_count);
    ClassDB::bind_method(D_METHOD("is_empty"), &ThetaStar3D::is_empty);
    ClassDB::bind_method(D_METHOD("get_capacity"), &ThetaStar3D::get_capacity);
    ClassDB::bind_method(D_METHOD("get_point_id", "position"), &ThetaStar3D::get_point_id);
    ClassDB::bind_method(D_METHOD("get_point_position", "id"), &ThetaStar3D::get_point_position);
    ClassDB::bind_method(D_METHOD("get_point_hash", "position"), &ThetaStar3D::get_point_hash);
    ClassDB::bind_method(D_METHOD("is_position_valid_for_hashing", "position"), &ThetaStar3D::is_position_valid_for_hashing);
    ClassDB::bind_method(D_METHOD("is_id_disabled", "id"), &ThetaStar3D::is_id_disabled);
    ClassDB::bind_method(D_METHOD("is_position_disabled", "id"), &ThetaStar3D::is_position_disabled);
    ClassDB::bind_method(D_METHOD("has_id", "id"), &ThetaStar3D::has_id);
    ClassDB::bind_method(D_METHOD("has_point", "position"), &ThetaStar3D::has_point);
    ClassDB::bind_method(D_METHOD("get_points"), &ThetaStar3D::get_points);
    ClassDB::bind_method(D_METHOD("get_point_connections", "position"), &ThetaStar3D::get_point_connections);
    ClassDB::bind_method(D_METHOD("get_id_path_from_positions", "from", "to"), &ThetaStar3D::get_id_path_from_positions);
    ClassDB::bind_method(D_METHOD("get_point_path_from_positions", "from", "to"), &ThetaStar3D::get_point_path_from_positions);
    ClassDB::bind_method(D_METHOD("get_id_path_from_ids", "from", "to"), &ThetaStar3D::get_id_path_from_ids);
    ClassDB::bind_method(D_METHOD("get_point_path_from_ids", "from", "to"), &ThetaStar3D::get_point_path_from_ids);
    ClassDB::bind_method(D_METHOD("get_point_path_from_off_graph_positions", "from", "to"), &ThetaStar3D::get_point_path_from_off_graph_positions);
    ClassDB::bind_method(D_METHOD("set_dimensions", "dimensions"), &ThetaStar3D::set_dimensions);
    ClassDB::bind_method(D_METHOD("enable_line_of_sight_check", "value"), &ThetaStar3D::enable_line_of_sight_check, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("add_point", "position"), &ThetaStar3D::add_point, DEFVAL(1.0));
    ClassDB::bind_method(D_METHOD("remove_point", "position"), &ThetaStar3D::remove_point);
    ClassDB::bind_method(D_METHOD("disable_point_by_position", "position", "disabled"), &ThetaStar3D::disable_point_by_position, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("disable_point_by_id", "id", "disabled"), &ThetaStar3D::disable_point_by_id);
    ClassDB::bind_method(D_METHOD("connect_points", "from", "to", "bidirectional"), &ThetaStar3D::connect_points, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("disconnect_points", "from", "to", "bidirectional"), &ThetaStar3D::disconnect_points, DEFVAL(false));

    GDVIRTUAL_BIND(_has_line_of_sight, "from", "to");
    GDVIRTUAL_BIND(_hash_position, "position");
    GDVIRTUAL_BIND(_compute_edge_cost, "from", "to");
    GDVIRTUAL_BIND(_estimate_edge_cost, "from", "to");

    // ADD_PROPERTY(PropertyInfo(Variant::VECTOR3I, "dimensions"), "set_dimensions", "get_dimensions");
}


bool ThetaStar3D::_are_dimensions_valid(const Vector3i& in_dimensions) {
    bool result = false;

    result = (
            in_dimensions.x > 0
            && in_dimensions.y > 0
            && in_dimensions.z > 0
    );

    return result;
}


bool ThetaStar3D::_connect_points(const int64_t from_id, const int64_t to_id, const bool bidirectional) {
    bool result = false;

    Point<Vector3i>* from_point = nullptr;
    Point<Vector3i>* to_point = nullptr;

    bool from_exists = _points.lookup(from_id, from_point);
    bool to_exists = _points.lookup(to_id, to_point);

    ERR_FAIL_COND_V_MSG(!from_exists, from_exists, vformat("Point 'from' doesn't exist. id = %d", from_id));
    ERR_FAIL_COND_V_MSG(!to_exists, to_exists, vformat("Point 'to' doesn't exist. id = %d", to_id));

    result = true;

    from_point->neighbors.set(to_id, to_point);

    if (bidirectional)
    {
        to_point->neighbors.set(from_id, from_point);
    }

    return result;
}


Point<Vector3i>* ThetaStar3D::_get_closest_point_toward(const Vector3 from, const Vector3 to) {
    Point<Vector3i>* result = nullptr;
    Vector3i from_on_graph;
    int64_t from_id = -1;
    Vector3 direction_to = from.direction_to(to);

    from_on_graph = _get_closest_position_toward(from, to);

    from_id = _hash_position(from_on_graph);

    ERR_FAIL_COND_V_MSG(!_points.lookup(from_id, result), result, vformat("Off-graph position too far from graph to find closest position. off-graph position = %v, rounded position = %v", from, from_on_graph));

    return result;
}


Vector3i ThetaStar3D::_get_closest_position_toward(const Vector3 from, const Vector3 to) const
{
    Vector3i result;
    Vector3 resultf;
    Vector3 max = Vector3(_dimensions) - Vector3(1.0, 1.0, 1.0);
    Vector3 min = _get_minimum_dimensions();
    Vector3 direction_to = from.direction_to(to);

    for (size_t i = 0; i < Vector3::AXIS_COUNT; i++) {
        if (from[i] > max[i]) {
            resultf[i] = max[i];
        } else if (from[i] < min[i]) {
            resultf[i] = min[i];
        } else {
            resultf[i] = direction_to[i] > 0.0 ? ceilf(from[i]) : floorf(from[i]);
        }
    }

    result = Vector3i(resultf);

    return result;
}


void ThetaStar3D::_get_point_path(Point<Vector3i>* const from, Point<Vector3i>* const to, LocalVector<const Point<Vector3i>*>& outPath) {
    LocalVector<Point<Vector3i>*> open;
    SortArray<Point<Vector3i>*, Point<Vector3i>::Comparator> sorter;
    bool is_path_found = false;

    _closed_counter++;

    from->cost_from_start = 0;
    from->cost_to_target = _estimate_edge_cost(from->id, to->id);

    open.push_back(from);

    while (!is_path_found && !open.is_empty()) {
        Point<Vector3i>* current = open[0];

        is_path_found = current == to;

        if (!is_path_found) {
            sorter.pop_heap(0, open.size(), open.ptr());
            open.remove_at(open.size() - 1);

            current->closed_counter = _closed_counter;

            _expand_point(current, to, open, sorter);
        }
    }

    if (is_path_found) {
        const Point<Vector3i>* point = to;
        while (point != from) {
            outPath.push_back(point);
            point = point->previous_point;
        }

        outPath.push_back(point);
        outPath.invert();
    }
}


void ThetaStar3D::_expand_point(Point<Vector3i>* const point, const Point<Vector3i>* const to, LocalVector<Point<Vector3i>*>& open, SortArray<Point<Vector3i>*, Point<Vector3i>::Comparator>& sorter) {
    for (OAHashMap<int64_t, Point<Vector3i>*>::Iterator it = point->neighbors.iter(); it.valid; it = point->neighbors.next_iter(it)) {
        Point<Vector3i>* const neighbor = *it.value;

        if (
                point->previous_point != nullptr
                && (is_line_of_sight_check_enabled && _has_line_of_sight(point->previous_point->position, neighbor->position))
        ) {
            _expand_point_helper(point->previous_point, neighbor, to, open, sorter);

        } else {
            _expand_point_helper(point, neighbor, to, open, sorter);
        }
    }
}


void ThetaStar3D::_expand_point_helper(Point<Vector3i>* const previous_point, Point<Vector3i>* const neighbor, const Point<Vector3i>* const to, LocalVector<Point<Vector3i>*>& open, SortArray<Point<Vector3i>*, Point<Vector3i>::Comparator>& sorter) {
    real_t neighbor_cost_from_start = previous_point->cost_from_start + _compute_edge_cost(previous_point->id, neighbor->id);

    bool use_neighbor_for_path_finding = neighbor->enabled;

    if (use_neighbor_for_path_finding) {
        use_neighbor_for_path_finding = (
                neighbor_cost_from_start < neighbor->cost_from_start
                || neighbor->opened_counter != _closed_counter
        );
    }

    if (use_neighbor_for_path_finding) {
        neighbor->cost_from_start = neighbor_cost_from_start;
        neighbor->cost_to_target = _estimate_edge_cost(neighbor->id, to->id);
        neighbor->previous_point = previous_point;

        if (neighbor->opened_counter != _closed_counter) {
            open.push_back(neighbor);
            neighbor->opened_counter = _closed_counter;

            sorter.push_heap(0, open.size() - 1, 0, neighbor, open.ptr());
        } else {
            sorter.push_heap(0, open.find(neighbor), 0, neighbor, open.ptr());
        }
    }
}


bool ThetaStar3D::_has_line_of_sight_helper(LineOfSightArguments& args) { // todo:: I don't think this function is quite right
    bool result = true;

    int64_t id_to_check_1 = _hash_position(args.voxel_to_check_1);
    Point<Vector3i>* point_to_check_1 = nullptr;

    int64_t id_to_check_2 = _hash_position(args.voxel_to_check_2);
    Point<Vector3i>* point_to_check_2 = nullptr;

    int64_t id_to_check_3 = _hash_position(args.voxel_to_check_3);
    Point<Vector3i>* point_to_check_3 = nullptr;

    args.balance += args.get_small_axis_distance();

    if (args.balance >= args.get_big_axis_distance()) {
        bool point_to_check_1_exists = _points.lookup(id_to_check_1, point_to_check_1);

        // only check if point if it is enabled if it exists, otherwise just assume the space if see-through
        if (point_to_check_1_exists) {
            result = point_to_check_1->enabled;
        }

        if (result) {
            args.small_axis_from += args.get_small_axis_sign();
            args.balance -= args.get_big_axis_distance();
        }
    }

    if (result) {
        if (args.balance != 0) {
            bool point_to_check_1_exists = _points.lookup(id_to_check_1, point_to_check_1);

            // only check if point if it is enabled if it exists, otherwise just assume the space if see-through
            if (point_to_check_1_exists) {
                result = point_to_check_1->enabled;
            }
        }
    }

    if (result) {
        if (args.get_small_axis_distance() == 0) {
            bool point_to_check_2_exists = _points.lookup(id_to_check_2, point_to_check_2);
            bool point_to_check_3_exists = _points.lookup(id_to_check_3, point_to_check_3);

            // only check if point if it is enabled if it exists, otherwise just assume the space if see-through
            if (point_to_check_2_exists) {
                result = point_to_check_2->enabled;
            }

            if (result && point_to_check_3_exists) {
                result = point_to_check_3->enabled;
            }
        }
    }

    return result;
}


bool ThetaStar3D::_has_line_of_sight(Vector3i from, Vector3i to) {
    bool result = false;

    if (GDVIRTUAL_CALL(_has_line_of_sight, from, to, result)) {
        return result;
    }

    int32_t from_x = from.x;
    int32_t from_y = from.y;
    int32_t from_z = from.z;

    int32_t to_x = to.x;
    int32_t to_y = to.y;
    int32_t to_z = to.z;

    int32_t distance_x = to_x - from_x;
    int32_t distance_y = to_y - from_y;
    int32_t distance_z = to_z - from_z;

    int32_t sign_x = 0;
    int32_t sign_y = 0;
    int32_t sign_z = 0;

    bool no_collision = true;
    // bool small_axes_arrived = false;

    if (distance_x < 0) {
        distance_x = -distance_x;
        sign_x = -1;
    } else if (distance_x > 0) {
        sign_x = 1;
    }

    if (distance_y < 0) {
        distance_y = -distance_y;
        sign_y = -1;
    } else if (distance_y > 0) {
        sign_y = 1;
    }

    if (distance_z < 0) {
        distance_z = -distance_z;
        sign_z = -1;
    } else if (distance_z > 0) {
        sign_z = 1;
    }

    // we are moving farthest along x-axis
    if (distance_x >= distance_y && distance_x >= distance_z) {
        LineOfSightArguments y_args(
                from.y,
                distance_x,
                distance_y,
                sign_y
        );

        LineOfSightArguments z_args(
                from.z,
                distance_x,
                distance_z,
                sign_z
        );

        while (no_collision && from_x != to_x) {
            Vector3i tmp;

            tmp.x = from_x + ((sign_x - 1) / 2); // checked twice? redundant?
            tmp.y = y_args.small_axis_from + ((y_args.get_small_axis_sign() - 1) / 2);
            tmp.z = z_args.small_axis_from + ((z_args.get_small_axis_sign() - 1) / 2);
            y_args.voxel_to_check_1 = tmp;

            tmp.y = y_args.small_axis_from + y_args.get_small_axis_sign();
            y_args.voxel_to_check_2 = tmp;

            tmp.y = y_args.small_axis_from + y_args.get_small_axis_sign() - 1;
            y_args.voxel_to_check_3 = tmp;


            tmp.x = from_x + ((sign_x - 1) / 2); // checked twice? redundant?
            tmp.y = y_args.small_axis_from + ((y_args.get_small_axis_sign() - 1) / 2);
            tmp.z = z_args.small_axis_from + ((z_args.get_small_axis_sign() - 1) / 2);
            z_args.voxel_to_check_1 = tmp;

            tmp.z = z_args.small_axis_from + z_args.get_small_axis_sign();
            z_args.voxel_to_check_2 = tmp;

            tmp.z = z_args.small_axis_from + z_args.get_small_axis_sign() - 1;
            z_args.voxel_to_check_3 = tmp;

            no_collision = _has_line_of_sight_helper(y_args);

            if (no_collision) {
                no_collision = _has_line_of_sight_helper(z_args);
            }

            from_x += sign_x;
            // from_z += sign_z;
            // small_axes_arrived = from_x == to_x && from_z == to_z;
        }

    // we are moving farthest along y-axis
    } else if (distance_y >= distance_x && distance_y >= distance_z) {
        LineOfSightArguments x_args(
                from.x,
                distance_y,
                distance_x,
                sign_x
        );

        LineOfSightArguments z_args(
                from.z,
                distance_y,
                distance_z,
                sign_z
        );

        while (no_collision && from_y != to_y) {
            Vector3i tmp;

            tmp.x = x_args.small_axis_from + ((x_args.get_small_axis_sign() - 1) / 2); // checked twice? redundant?
            tmp.y = from_y + ((sign_y - 1) / 2);
            tmp.z = z_args.small_axis_from + ((z_args.get_small_axis_sign() - 1) / 2);
            x_args.voxel_to_check_1 = tmp;

            tmp.x = x_args.small_axis_from + x_args.get_small_axis_sign(); // this should not be checked if distance_x == 0
            x_args.voxel_to_check_2 = tmp;

            tmp.x = x_args.small_axis_from + x_args.get_small_axis_sign() - 1;
            x_args.voxel_to_check_3 = tmp;


            tmp.x = x_args.small_axis_from + ((x_args.get_small_axis_sign() - 1) / 2); // checked twice? redundant?
            tmp.y = from_y + ((sign_y - 1) / 2);
            tmp.z = z_args.small_axis_from + ((z_args.get_small_axis_sign() - 1) / 2);
            z_args.voxel_to_check_1 = tmp;

            tmp.z = z_args.small_axis_from + z_args.get_small_axis_sign(); // this should not be checked if distance_z == 0
            z_args.voxel_to_check_2 = tmp;

            tmp.z = z_args.small_axis_from + z_args.get_small_axis_sign() - 1;
            z_args.voxel_to_check_3 = tmp;

            no_collision = _has_line_of_sight_helper(x_args);

            if (no_collision) {
                no_collision = _has_line_of_sight_helper(z_args);
            }

            from_y += sign_y;
            // from_z += sign_z;
            // small_axes_arrived = from_y == to_y && from_z == to_z;
        }

    // we are moving farthest along z-axis
    } else if (distance_z >= distance_x && distance_z >= distance_y) {
        LineOfSightArguments x_args(
                from.x,
                distance_z,
                distance_x,
                sign_x
        );

        LineOfSightArguments y_args(
                from.y,
                distance_z,
                distance_y,
                sign_y
        );

        while (no_collision && from_x != to_x) {
            Vector3i tmp;

            tmp.x = x_args.small_axis_from + ((x_args.get_small_axis_sign() - 1) / 2); // checked twice? redundant?
            tmp.y = y_args.small_axis_from + ((y_args.get_small_axis_sign() - 1) / 2);
            tmp.z = from_z + ((sign_z - 1) / 2);
            x_args.voxel_to_check_1 = tmp;

            tmp.x = x_args.small_axis_from + x_args.get_small_axis_sign();
            x_args.voxel_to_check_2 = tmp;

            tmp.x = x_args.small_axis_from + x_args.get_small_axis_sign() - 1;
            x_args.voxel_to_check_3 = tmp;

            tmp.x = x_args.small_axis_from + ((x_args.get_small_axis_sign() - 1) / 2); // checked twice? redundant?
            tmp.y = y_args.small_axis_from + ((y_args.get_small_axis_sign() - 1) / 2);
            tmp.z = from_z + ((sign_z - 1) / 2);
            y_args.voxel_to_check_1 = tmp;

            tmp.y = y_args.small_axis_from + y_args.get_small_axis_sign();
            y_args.voxel_to_check_2 = tmp;

            tmp.y = y_args.small_axis_from + y_args.get_small_axis_sign() - 1;
            y_args.voxel_to_check_3 = tmp;

            no_collision = _has_line_of_sight_helper(x_args);

            if (no_collision) {
                no_collision = _has_line_of_sight_helper(y_args);
            }

            from_x += sign_x;
            // from_y += sign_y;
            // small_axes_arrived = from_x == to_x && from_y == to_y;
        }
    }

    result = no_collision;

    return result;
}


int64_t ThetaStar3D::_hash_position(Vector3i position) {
    int64_t result = -1;

    if (GDVIRTUAL_CALL(_hash_position, position, result)) {
        return result;
    }

	int64_t dimension_x = _dimensions.x;
	int64_t dimension_y = _dimensions.y;

	dimension_x <<= 1;
	dimension_y <<= 1;

	int64_t x = position.x;
	int64_t y = position.y;
	int64_t z = position.z;

	x += _dimensions.x - 1;
	y += _dimensions.y - 1;
	z += _dimensions.z - 1;

	result = x + (dimension_x * y) + (dimension_x * dimension_y * z);

    return result;
}


real_t ThetaStar3D::_compute_edge_cost(int64_t from, int64_t to) {
    real_t result = 0.0;

    if (GDVIRTUAL_CALL(_compute_edge_cost, from, to, result)) {
        return result;
    }

    Point<Vector3i>* from_point;
    Point<Vector3i>* to_point;

    _points.lookup(from, from_point);
    _points.lookup(to, to_point);

    Vector3 from_position = Vector3(from_point->position);
    Vector3 to_position = Vector3(to_point->position);

    result = from_position.distance_to(to_position);

    return result;
}


real_t ThetaStar3D::_estimate_edge_cost(int64_t from, int64_t to) {
    real_t result = 0.0;

    if (GDVIRTUAL_CALL(_estimate_edge_cost, from, to, result)) {
        return result;
    }

    Point<Vector3i>* from_point;
    Point<Vector3i>* to_point;

    _points.lookup(from, from_point);
    _points.lookup(to, to_point);

    Vector3 from_position = Vector3(from_point->position);
    Vector3 to_position = Vector3(to_point->position);

    result = floorf(from_position.distance_to(to_position));

    return result;
}


bool ThetaStar3D::_is_position_valid(const Vector3i position, bool warn) const {
    bool result = false;
    Vector3i max = _dimensions - Vector3i(1, 1, 1);
    Vector3i min = _get_minimum_dimensions();
    Vector3i clamped_position = position.clamp(min, max);

    result = position == clamped_position;

    if (warn) {
        ERR_FAIL_COND_V_MSG(!result, result, vformat("Point's position must be within _dimensions. position = %v", position));
    }

    return result;
}


Vector3i ThetaStar3D::_get_minimum_dimensions() const {
    Vector3i result;

    result = -_dimensions + Vector3i(1, 1, 1);

    return result;
}

};
