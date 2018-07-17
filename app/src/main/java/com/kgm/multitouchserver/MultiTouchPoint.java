package com.kgm.multitouchserver;

public class MultiTouchPoint {
    public char state;
    public int id;
    public int shadowId;
    public int x;
    public int y;

    public MultiTouchPoint() {}

    public MultiTouchPoint(String pointString)
    {
        String[] p = pointString.split(",");
        if (p.length == 4) {
            state = p[0].charAt(0);
            id = shadowId = Integer.parseInt(p[1]);
            x = Integer.parseInt(p[2]);
            y = Integer.parseInt(p[3]);
            // Log.i("MultiTouchPoint", String.format(" --> state=%s, id=%d, x=%d, y=%d", state, id, x, y));
        }
        else
        {
            throw new IllegalArgumentException("Invalid point string [" + pointString + "] does not contain four substrings");
        }
    }

    public MultiTouchPoint(char a_state, int a_id, int a_x, int a_y)
    {
        state = a_state;
        id = shadowId = a_id;
        x = a_x;
        y = a_y;
    }
    //
    // Custom equals() so we can do vector searches on id
    //
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (!(obj instanceof MultiTouchPoint)) return false;
        MultiTouchPoint m_obj = (MultiTouchPoint)obj;
        return id == m_obj.id;
    }

    //
    // Custom hashCode() to match the custom equals()
    //
    @Override
    public int hashCode() {
        return id;
    }

    @Override
    public String toString() {
        return String.format("state='" + state + "',id=" + id + ",shadowId=" + shadowId + ",x=" + x + ",y=" + y);
    }

}
