/**************************************************************************/
/*  aabb.h                                                                */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             REDOT ENGINE                               */
/*                        https://redotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2024-present Redot Engine contributors                   */
/*                                          (see REDOT_AUTHORS.md)        */
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef AABBI_H
#define AABBI_H

#include "core/math/plane.h"
#include "core/math/vector3.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"

/**
 * AABBi (Axis Aligned Bounding Box integer)
 * This is implemented by a point (position) and the box size.
 */

class Variant;

class AABBi : public RefCounted {
	GDCLASS(AABBi, RefCounted);

public:
	Vector3i position;
	Vector3i size;

	int32_t get_volume() const;
	_FORCE_INLINE_ bool has_volume() const {
		return size.x > 0 && size.y > 0 && size.z > 0;
	}

	_FORCE_INLINE_ bool has_surface() const {
		return size.x > 0 || size.y > 0 || size.z > 0;
	}

	const Vector3i &get_position() const { return position; }
	void set_position(const Vector3i &p_pos) { position = p_pos; }
	const Vector3i &get_size() const { return size; }
	void set_size(const Vector3i &p_size) { size = p_size; }

	bool operator==(const AABBi &p_rval) const;
	bool operator!=(const AABBi &p_rval) const;

	bool is_equal_approx(const AABBi &p_aabb) const;
	bool is_finite() const;
	_FORCE_INLINE_ bool intersects(const AABBi &p_aabb) const; /// Both AABBs overlap
	_FORCE_INLINE_ bool intersects_inclusive(const AABBi &p_aabb) const; /// Both AABBs (or their faces) overlap
	_FORCE_INLINE_ bool encloses(const AABBi &p_aabb) const; /// p_aabb is completely inside this

	AABBi merge(const AABBi &p_with) const;
	void merge_with(const AABBi &p_aabb); ///merge with another AABBi
	AABBi intersection(const AABBi &p_aabb) const; ///get box where two intersect, empty if no intersection occurs
	_FORCE_INLINE_ bool smits_intersect_ray(const Vector3 &p_from, const Vector3 &p_dir, real_t p_t0, real_t p_t1) const;

	bool intersects_segment(const Vector3i &p_from, const Vector3i &p_to, Vector3i *r_intersection_point = nullptr, Vector3i *r_normal = nullptr) const;
	bool intersects_segment(const Vector3 &p_from, const Vector3 &p_to, Vector3 *r_intersection_point = nullptr, Vector3 *r_normal = nullptr) const;
	bool intersects_ray(const Vector3 &p_from, const Vector3 &p_dir) const {
		bool inside;
		return find_intersects_ray(p_from, p_dir, inside);
	}

	bool find_intersects_ray(const Vector3 &p_from, const Vector3 &p_dir, bool &r_inside, Vector3 *r_intersection_point = nullptr, Vector3 *r_normal = nullptr) const;

	_FORCE_INLINE_ bool intersects_convex_shape(const Plane *p_planes, int p_plane_count, const Vector3 *p_points, int p_point_count) const;
	_FORCE_INLINE_ bool inside_convex_shape(const Plane *p_planes, int p_plane_count) const;
	bool intersects_plane(const Plane &p_plane) const;

	_FORCE_INLINE_ bool has_point(const Vector3i &p_point) const;
	_FORCE_INLINE_ bool has_point(const Vector3 &p_point) const;
	_FORCE_INLINE_ Vector3i get_support(const Vector3i &p_direction) const;

	Vector3i get_longest_axis() const;
	int get_longest_axis_index() const;
	_FORCE_INLINE_ int32_t get_longest_axis_size() const;

	Vector3i get_shortest_axis() const;
	int get_shortest_axis_index() const;
	_FORCE_INLINE_ int32_t get_shortest_axis_size() const;

	AABBi grow(int32_t p_by) const;
	_FORCE_INLINE_ void grow_by(int32_t p_amount);

	void get_edge(int p_edge, Vector3i &r_from, Vector3i &r_to) const;
	_FORCE_INLINE_ Vector3i get_endpoint(int p_point) const;

	AABBi expand(const Vector3i &p_vector) const;
	_FORCE_INLINE_ void project_range_in_plane(const Plane &p_plane, int32_t &r_min, int32_t &r_max) const;
	_FORCE_INLINE_ void expand_to(const Vector3i &p_vector); /** expand to contain a point if necessary */

	_FORCE_INLINE_ AABBi abs() const {
		return AABBi(position + size.mini(0), size.abs());
	}

	Variant intersects_segment_bind(const Vector3i &p_from, const Vector3i &p_to) const;
	Variant intersects_ray_bind(const Vector3 &p_from, const Vector3 &p_dir) const;

	_FORCE_INLINE_ void quantize(int32_t p_unit);
	_FORCE_INLINE_ AABBi quantized(int32_t p_unit) const;

	_FORCE_INLINE_ void set_end(const Vector3i &p_end) {
		size = p_end - position;
	}

	_FORCE_INLINE_ Vector3i get_end() const {
		return position + size;
	}

	_FORCE_INLINE_ Vector3i get_center() const {
		return position + (Vector3(size) * 0.5f).floor();
	}

	operator String() const;

	_FORCE_INLINE_ AABBi() {}
	_FORCE_INLINE_ AABBi(const AABBi &p_other) {}
	inline AABBi(const Vector3i &p_pos, const Vector3i &p_size) :
			position(p_pos),
			size(p_size) {
	}

protected:
	static void _bind_methods();
};

inline bool AABBi::intersects(const AABBi &p_aabb) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabb.size.x < 0 || p_aabb.size.y < 0 || p_aabb.size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	if (position.x >= (p_aabb.position.x + p_aabb.size.x)) {
		return false;
	}
	if ((position.x + size.x) <= p_aabb.position.x) {
		return false;
	}
	if (position.y >= (p_aabb.position.y + p_aabb.size.y)) {
		return false;
	}
	if ((position.y + size.y) <= p_aabb.position.y) {
		return false;
	}
	if (position.z >= (p_aabb.position.z + p_aabb.size.z)) {
		return false;
	}
	if ((position.z + size.z) <= p_aabb.position.z) {
		return false;
	}

	return true;
}

inline bool AABBi::intersects_inclusive(const AABBi &p_aabb) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabb.size.x < 0 || p_aabb.size.y < 0 || p_aabb.size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	if (position.x > (p_aabb.position.x + p_aabb.size.x)) {
		return false;
	}
	if ((position.x + size.x) < p_aabb.position.x) {
		return false;
	}
	if (position.y > (p_aabb.position.y + p_aabb.size.y)) {
		return false;
	}
	if ((position.y + size.y) < p_aabb.position.y) {
		return false;
	}
	if (position.z > (p_aabb.position.z + p_aabb.size.z)) {
		return false;
	}
	if ((position.z + size.z) < p_aabb.position.z) {
		return false;
	}

	return true;
}

inline bool AABBi::encloses(const AABBi &p_aabb) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabb.size.x < 0 || p_aabb.size.y < 0 || p_aabb.size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	Vector3i src_min = position;
	Vector3i src_max = position + size;
	Vector3i dst_min = p_aabb.position;
	Vector3i dst_max = p_aabb.position + p_aabb.size;

	return (
			(src_min.x <= dst_min.x) &&
			(src_max.x >= dst_max.x) &&
			(src_min.y <= dst_min.y) &&
			(src_max.y >= dst_max.y) &&
			(src_min.z <= dst_min.z) &&
			(src_max.z >= dst_max.z));
}

Vector3i AABBi::get_support(const Vector3i &p_direction) const {
	Vector3i support = position;
	if (p_direction.x > 0.0f) {
		support.x += size.x;
	}
	if (p_direction.y > 0.0f) {
		support.y += size.y;
	}
	if (p_direction.z > 0.0f) {
		support.z += size.z;
	}
	return support;
}

Vector3i AABBi::get_endpoint(int p_point) const {
	switch (p_point) {
		case 0:
			return Vector3i(position.x, position.y, position.z);
		case 1:
			return Vector3i(position.x, position.y, position.z + size.z);
		case 2:
			return Vector3i(position.x, position.y + size.y, position.z);
		case 3:
			return Vector3i(position.x, position.y + size.y, position.z + size.z);
		case 4:
			return Vector3i(position.x + size.x, position.y, position.z);
		case 5:
			return Vector3i(position.x + size.x, position.y, position.z + size.z);
		case 6:
			return Vector3i(position.x + size.x, position.y + size.y, position.z);
		case 7:
			return Vector3i(position.x + size.x, position.y + size.y, position.z + size.z);
	}

	ERR_FAIL_V(Vector3i());
}

bool AABBi::intersects_convex_shape(const Plane *p_planes, int p_plane_count, const Vector3 *p_points, int p_point_count) const {
	Vector3i half_extents = size * 0.5f;
	Vector3i ofs = position + half_extents;

	for (int i = 0; i < p_plane_count; i++) {
		const Plane &p = p_planes[i];
		Vector3i point(
				(p.normal.x > 0) ? -half_extents.x : half_extents.x,
				(p.normal.y > 0) ? -half_extents.y : half_extents.y,
				(p.normal.z > 0) ? -half_extents.z : half_extents.z);
		point += ofs;
		if (p.is_point_over(point)) {
			return false;
		}
	}

	// Make sure all points in the shape aren't fully separated from the AABBi on
	// each axis.
	int bad_point_counts_positive[3] = { 0 };
	int bad_point_counts_negative[3] = { 0 };

	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < p_point_count; i++) {
			if (p_points[i].coord[k] > ofs.coord[k] + half_extents.coord[k]) {
				bad_point_counts_positive[k]++;
			}
			if (p_points[i].coord[k] < ofs.coord[k] - half_extents.coord[k]) {
				bad_point_counts_negative[k]++;
			}
		}

		if (bad_point_counts_negative[k] == p_point_count) {
			return false;
		}
		if (bad_point_counts_positive[k] == p_point_count) {
			return false;
		}
	}

	return true;
}

bool AABBi::inside_convex_shape(const Plane *p_planes, int p_plane_count) const {
	Vector3i half_extents = size * 0.5f;
	Vector3i ofs = position + half_extents;

	for (int i = 0; i < p_plane_count; i++) {
		const Plane &p = p_planes[i];
		Vector3i point(
				(p.normal.x < 0) ? -half_extents.x : half_extents.x,
				(p.normal.y < 0) ? -half_extents.y : half_extents.y,
				(p.normal.z < 0) ? -half_extents.z : half_extents.z);
		point += ofs;
		if (p.is_point_over(point)) {
			return false;
		}
	}

	return true;
}

bool AABBi::has_point(const Vector3i &p_point) const {
	return has_point(Vector3(p_point));
}

bool AABBi::has_point(const Vector3 &p_point) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	if (p_point.x < static_cast<real_t>(position.x)) {
		return false;
	}
	if (p_point.y <static_cast<real_t>(position.y)) {
		return false;
	}
	if (p_point.z < static_cast<real_t>(position.z)) {
		return false;
	}
	if (p_point.x > static_cast<real_t>(position.x + size.x)) {
		return false;
	}
	if (p_point.y > static_cast<real_t>(position.y + size.y)) {
		return false;
	}
	if (p_point.z > static_cast<real_t>(position.z + size.z)) {
		return false;
	}

	return true;
}

inline void AABBi::expand_to(const Vector3i &p_vector) {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	Vector3i begin = position;
	Vector3i end = position + size;

	if (p_vector.x < begin.x) {
		begin.x = p_vector.x;
	}
	if (p_vector.y < begin.y) {
		begin.y = p_vector.y;
	}
	if (p_vector.z < begin.z) {
		begin.z = p_vector.z;
	}

	if (p_vector.x > end.x) {
		end.x = p_vector.x;
	}
	if (p_vector.y > end.y) {
		end.y = p_vector.y;
	}
	if (p_vector.z > end.z) {
		end.z = p_vector.z;
	}

	position = begin;
	size = end - begin;
}

void AABBi::project_range_in_plane(const Plane &p_plane, int32_t &r_min, int32_t &r_max) const {
	Vector3i half_extents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
	Vector3i center(position.x + half_extents.x, position.y + half_extents.y, position.z + half_extents.z);

	int32_t length = p_plane.normal.abs().dot(half_extents);
	int32_t distance = p_plane.distance_to(center);
	r_min = distance - length;
	r_max = distance + length;
}

inline int32_t AABBi::get_longest_axis_size() const {
	int32_t max_size = size.x;

	if (size.y > max_size) {
		max_size = size.y;
	}

	if (size.z > max_size) {
		max_size = size.z;
	}

	return max_size;
}

inline int32_t AABBi::get_shortest_axis_size() const {
	int32_t max_size = size.x;

	if (size.y < max_size) {
		max_size = size.y;
	}

	if (size.z < max_size) {
		max_size = size.z;
	}

	return max_size;
}

bool AABBi::smits_intersect_ray(const Vector3 &p_from, const Vector3 &p_dir, real_t p_t0, real_t p_t1) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABBi size is negative, this is not supported. Use AABBi.abs() to get an AABBi with a positive size.");
	}
#endif
	real_t divx = 1.0f / p_dir.x;
	real_t divy = 1.0f / p_dir.y;
	real_t divz = 1.0f / p_dir.z;

	Vector3 lposition = Vector3(position);
	Vector3 upbound = Vector3(position + size);
	real_t tmin, tmax, tymin, tymax, tzmin, tzmax;
	if (p_dir.x >= 0) {
		tmin = (lposition.x - p_from.x) * divx;
		tmax = (upbound.x - p_from.x) * divx;
	} else {
		tmin = (upbound.x - p_from.x) * divx;
		tmax = (lposition.x - p_from.x) * divx;
	}
	if (p_dir.y >= 0) {
		tymin = (lposition.y - p_from.y) * divy;
		tymax = (upbound.y - p_from.y) * divy;
	} else {
		tymin = (upbound.y - p_from.y) * divy;
		tymax = (lposition.y - p_from.y) * divy;
	}
	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}
	if (p_dir.z >= 0) {
		tzmin = (lposition.z - p_from.z) * divz;
		tzmax = (upbound.z - p_from.z) * divz;
	} else {
		tzmin = (upbound.z - p_from.z) * divz;
		tzmax = (lposition.z - p_from.z) * divz;
	}
	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}
	return ((tmin < p_t1) && (tmax > p_t0));
}

void AABBi::grow_by(int32_t p_amount) {
	position.x -= p_amount;
	position.y -= p_amount;
	position.z -= p_amount;
	size.x += 2 * p_amount;
	size.y += 2 * p_amount;
	size.z += 2 * p_amount;
}

void AABBi::quantize(int32_t p_unit) {
	size += position;

	Vector3 lposition = Vector3(position);
	Vector3 lsize = Vector3(size);
	real_t lp_unit = static_cast<real_t>(p_unit);

	lposition.x -= Math::fposmodp(lposition.x, lp_unit);
	lposition.y -= Math::fposmodp(lposition.y, lp_unit);
	lposition.z -= Math::fposmodp(lposition.z, lp_unit);

	lsize.x -= Math::fposmodp(lsize.x, lp_unit);
	lsize.y -= Math::fposmodp(lsize.y, lp_unit);
	lsize.z -= Math::fposmodp(lsize.z, lp_unit);

	position = Vector3i(lposition);
	size = Vector3i(lsize);

	size.x += p_unit;
	size.y += p_unit;
	size.z += p_unit;

	size -= position;
}

AABBi AABBi::quantized(int32_t p_unit) const {
	AABBi ret = *this;
	ret.quantize(p_unit);
	return ret;
}

#endif // AABBI_H
