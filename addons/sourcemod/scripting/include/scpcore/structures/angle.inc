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

enum AngleVectorType {
    Forward = 0,
    Right,
    Up
}

methodmap Angle < Vector {
    
    public Angle(float x, float y, float z) {
        Angle ang = view_as<Angle>(new Base());

        ang.SetValue("x", x);
        ang.SetValue("y", y);
        ang.SetValue("z", z);
        
        return ang;
    }

    public Vector GetVectors(AngleVectorType type) {
        float angarr[3], vecarr[3];
        this.GetArr(angarr);

        if (type == Forward)
            GetAngleVectors(angarr, vecarr, NULL_VECTOR, NULL_VECTOR);
        else if (type == Right)
            GetAngleVectors(angarr, NULL_VECTOR, vecarr, NULL_VECTOR);
        else if (type == Up)
            GetAngleVectors(angarr, NULL_VECTOR, NULL_VECTOR, vecarr);

        delete this;

        return new Vector(vecarr[0], vecarr[1], vecarr[2]);
    }

    public Vector GetForwardVectorScaled(Vector startpos, float scale) {
        float startposarr[3], angarr[3], forwardarr[3], endposarr[3];
        startpos.GetArr(startposarr);
        this.GetArr(angarr);
        GetAngleVectors(angarr, forwardarr, NULL_VECTOR, NULL_VECTOR);
        ScaleVector(forwardarr, scale);
        AddVectors(startposarr, forwardarr, endposarr);

        delete this;
        delete startpos;

        return new Vector(endposarr[0], endposarr[1], endposarr[2]);
    }

    public Vector GetRightVectorScaled(Vector startpos, float scale) {
        float startposarr[3], angarr[3], rightarr[3], endposarr[3];
        startpos.GetArr(startposarr);
        this.GetArr(angarr);
        GetAngleVectors(angarr, NULL_VECTOR, rightarr, NULL_VECTOR);
        ScaleVector(rightarr, scale);
        AddVectors(startposarr, rightarr, endposarr);

        delete this;
        delete startpos;

        return new Vector(endposarr[0], endposarr[1], endposarr[2]);
    }

    public Vector GetUpVectorScaled(Vector startpos, float scale) {
        float startposarr[3], angarr[3], uparr[3], endposarr[3];
        startpos.GetArr(startposarr);
        this.GetArr(angarr);
        GetAngleVectors(angarr, NULL_VECTOR, NULL_VECTOR, uparr);
        ScaleVector(uparr, scale);
        AddVectors(startposarr, uparr, endposarr);

        delete this;
        delete startpos;

        return new Vector(endposarr[0], endposarr[1], endposarr[2]);
    }
}

stock Angle operator+(Angle first, Angle second) {
    Angle result = new Angle(first.x + second.x, first.y + second.y, first.z + second.z);
    delete first; delete second;
    return result;
}

stock Angle operator-(Angle first, Angle second) {
    Angle result = new Angle(first.x - second.x, first.y - second.y, first.z - second.z);
    delete first; delete second;
    return result;
}