/**
 * =============================================================================
 * Copyright (C) 2021 Eternity team (Andrey::Dono, GeTtOo).
 * =============================================================================
 *
 * This file is part of the SCP Breach CS:GO.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 **/

methodmap Vector < Base {

    public Vector(float x = 0.0, float y = 0.0, float z = 0.0) {
        Vector vec = view_as<Vector>(new Base());

        vec.SetValue("x", x);
        vec.SetValue("y", y);
        vec.SetValue("z", z);
        
        return vec;
    }

    property float x {
        public set(float x) { this.SetFloat("x", x); }
        public get() { return this.GetFloat("x"); }
    }

    property float y {
        public set(float y) { this.SetFloat("y", y); }
        public get() { return this.GetFloat("y"); }
    }

    property float z {
        public set(float z) { this.SetFloat("z", z); }
        public get() { return this.GetFloat("z"); }
    }

    public static Vector v(float val) { return new Vector(val, val, val); }

    public void SetArr(float vec[3]) {
        this.x = vec[0];
        this.y = vec[1];
        this.z = vec[2];
    }

    public void GetArr(float vec[3]) {
        vec[0] = this.x;
        vec[1] = this.y;
        vec[2] = this.z;
    }

    public void GetArrD(float vec[3]) {
        vec[0] = this.x;
        vec[1] = this.y;
        vec[2] = this.z;

        delete this;
    }

    public Vector Plus(Vector vec2) {
        this.x += vec2.x;
        this.y += vec2.y;
        this.z += vec2.z;

        delete vec2;

        return this;
    }

    public Vector Minus(Vector vec2) {
        this.x -= vec2.x;
        this.y -= vec2.y;
        this.z -= vec2.z;

        delete vec2;

        return this;
    }

    public Vector Clone() {
        return new Vector(this.x, this.y, this.z);
    }

    public float DotProduct(Vector vec2) {
        float vecarr1[3], vecarr2[3];
        this.GetArrD(vecarr1);
        vec2.GetArrD(vecarr2);

        float dotproduct = GetVectorDotProduct(vecarr1, vecarr2);

        return dotproduct;
    }

    public float Length() {
        float vecarr1[3];
        this.GetArrD(vecarr1);
        
        float length = GetVectorLength(vecarr1);

        return length;
    }

    public Vector GetFromPoint(Vector vec) {
        float vecarr1[3], vecarr2[3], vecarr3[3];
        this.GetArr(vecarr1);
        vec.GetArr(vecarr2);

        MakeVectorFromPoints(vecarr1, vecarr2, vecarr3);
        
        delete this;
        delete vec;

        return new Vector(vecarr3[0], vecarr3[1], vecarr3[2]);
    }

    public Vector Normalize() {
        float vecarr[3];
        this.GetArr(vecarr);
        NormalizeVector(vecarr, vecarr);
        this.SetArr(vecarr);

        return this;
    }

    public Vector Scale(float scale) {
        float vecarr[3];
        this.GetArr(vecarr);
        ScaleVector(vecarr, scale);
        this.SetArr(vecarr);

        return this;
    }

    public Vector Subtract(Vector vec) {
        float vecarr1[3], vecarr2[3];
        this.GetArr(vecarr1);
        vec.GetArrD(vecarr2);
        SubtractVectors(vecarr1, vecarr2, vecarr1);
        this.SetArr(vecarr1);

        return this;
    }

    public Vector Lerp(Vector vec, float percent) {
        float vecarr1[3], vecarr2[3], vecarr3[3];
        this.GetArr(vecarr1);
        vec.GetArrD(vecarr2);
        vecarr3[0] = vecarr1[0] + (vecarr2[0] - vecarr1[0]) * percent;
        vecarr3[1] = vecarr1[1] + (vecarr2[1] - vecarr1[1]) * percent;
        vecarr3[2] = vecarr1[2] + (vecarr2[2] - vecarr1[2]) * percent;
        this.SetArr(vecarr3);
        return this;
    }
    
    public any GetAngles() {
        float pos[3], ang[3];
        this.GetArr(pos);
        GetVectorAngles(pos, ang);
        this.SetArr(ang);
        return this;
    }
}

stock Vector operator+(Vector first, Vector second) {
    Vector result = new Vector(first.x + second.x, first.y + second.y, first.z + second.z);
    delete first; delete second;
    return result;
}

stock Vector operator-(Vector first, Vector second) {
    Vector result = new Vector(first.x - second.x, first.y - second.y, first.z - second.z);
    delete first; delete second;
    return result;
}

stock Vector operator*(Vector first, float val) {
    float vecarr1[3];
    first.GetArr(vecarr1);
    vecarr1[0] *= val;
    vecarr1[1] *= val;
    vecarr1[2] *= val;
    first.SetArr(vecarr1);
    return first;
}

stock bool operator>(Vector first, Vector second) {
    bool result = false;
    if ((first.x > second.x) && (first.y > second.y) && (first.z > second.z))
        result = true;
    delete first; delete second;
    return result;
}

stock bool operator<(Vector first, Vector second) {
    bool result = false;
    if ((first.x < second.x) && (first.y < second.y) && (first.z < second.z))
        result = true;
    delete first; delete second;
    return result;
}