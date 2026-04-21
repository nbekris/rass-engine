// File Name:    ISerializable.h
// Author(s):    main Taro Omiya, secondary Steven Yacoub
// Course:       GAM541
// Project:      RASS
// Purpose:      Interface for serializable objects.
//
// Copyright © 2026 DigiPen (USA) Corporation.

#pragma once

namespace RassEngine {

template<typename T>
class ISerializable {
public:
	virtual ~ISerializable() = default;

	/// <summary>
	/// Reads content from a resource,
	/// and update this object's content.
	/// </summary>
	virtual bool Read(T&) = 0;
};

}
