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

methodmap Colour < Base {

    public Colour(int r = 0, int g = 0, int b = 0, int a = 255) {
        Colour col = view_as<Colour>(new Base());

        col.SetValue("r", r);
        col.SetValue("g", g);
        col.SetValue("b", b);
        col.SetValue("a", a);
        
        return col;
    }

    property int r {
        public set(int r) { this.SetInt("r", r); }
        public get() { return this.GetInt("r"); }
    }

    property int g {
        public set(int g) { this.SetInt("g", g); }
        public get() { return this.GetInt("g"); }
    }

    property int b {
        public set(int b) { this.SetInt("b", b); }
        public get() { return this.GetInt("b"); }
    }

    property int a {
        public set(int a) { this.SetInt("a", a); }
        public get() { return this.GetInt("a"); }
    }

    public Colour Clone() {
        return new Colour(this.r, this.g, this.b, this.a);
    }

    public void GetArr(int[] clr) {
        clr[0] = this.r;
        clr[1] = this.g;
        clr[2] = this.b;
        clr[3] = this.a;
    }
}